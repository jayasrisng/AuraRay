#!/usr/bin/env swift

import AppKit
import Foundation
import ImageIO
import UniformTypeIdentifiers

enum MediaError: Error, CustomStringConvertible {
    case invalidArguments
    case imageLoadFailed(String)
    case outputCreationFailed(String)
    case noFrames(String)

    var description: String {
        switch self {
        case .invalidArguments:
            return "Usage: generate_demo_media.swift comparison <repo-root> <output.png> | gif <frames-dir> <output.gif>"
        case .imageLoadFailed(let path):
            return "Could not load image: \(path)"
        case .outputCreationFailed(let path):
            return "Could not create output: \(path)"
        case .noFrames(let path):
            return "No PNG frames found in: \(path)"
        }
    }
}

func loadImage(_ path: String) throws -> NSImage {
    guard let image = NSImage(contentsOfFile: path) else {
        throw MediaError.imageLoadFailed(path)
    }
    return image
}

func drawText(_ text: String, top: CGFloat, left: CGFloat, width: CGFloat, font: NSFont, color: NSColor, canvasHeight: CGFloat) {
    let attributes: [NSAttributedString.Key: Any] = [
        .font: font,
        .foregroundColor: color
    ]
    let height = font.pointSize * 1.35
    text.draw(in: NSRect(x: left, y: canvasHeight - top - height, width: width, height: height), withAttributes: attributes)
}

func writeComparison(repositoryRoot: String, outputPath: String) throws {
    let items = [
        ("Full Quality", "foveated_full.png", "64 samples across the image"),
        ("Low Quality", "foveated_low.png", "8 samples across the image"),
        ("Gaze-Aware", "foveated_gaze.png", "64 / 24 / 8 sample regions"),
        ("Overlay", "foveated_overlay.png", "simulated gaze and region boundaries")
    ]

    let canvasWidth = 1200
    let canvasHeight = 860
    guard let bitmap = NSBitmapImageRep(
        bitmapDataPlanes: nil,
        pixelsWide: canvasWidth,
        pixelsHigh: canvasHeight,
        bitsPerSample: 8,
        samplesPerPixel: 4,
        hasAlpha: true,
        isPlanar: false,
        colorSpaceName: .deviceRGB,
        bytesPerRow: 0,
        bitsPerPixel: 0
    ) else {
        throw MediaError.outputCreationFailed(outputPath)
    }

    NSGraphicsContext.saveGraphicsState()
    NSGraphicsContext.current = NSGraphicsContext(bitmapImageRep: bitmap)
    NSColor(calibratedRed: 0.025, green: 0.032, blue: 0.050, alpha: 1.0).setFill()
    NSRect(x: 0, y: 0, width: canvasWidth, height: canvasHeight).fill()

    drawText("AuraRay Foveated Rendering Comparison", top: 24, left: 40, width: 1120, font: .boldSystemFont(ofSize: 34), color: .white, canvasHeight: CGFloat(canvasHeight))
    drawText("The same XR-inspired sphere scene rendered with different sampling budgets", top: 70, left: 40, width: 1120, font: .systemFont(ofSize: 18), color: NSColor(calibratedWhite: 0.72, alpha: 1.0), canvasHeight: CGFloat(canvasHeight))

    let positions: [(CGFloat, CGFloat)] = [(40, 138), (612, 138), (40, 510), (612, 510)]
    let imageWidth: CGFloat = 548
    let imageHeight: CGFloat = 308

    for (index, item) in items.enumerated() {
        let (left, top) = positions[index]
        drawText(item.0, top: top - 34, left: left, width: imageWidth, font: .boldSystemFont(ofSize: 22), color: .white, canvasHeight: CGFloat(canvasHeight))
        let image = try loadImage(repositoryRoot + "/renders/" + item.1)
        let destination = NSRect(x: left, y: CGFloat(canvasHeight) - top - imageHeight, width: imageWidth, height: imageHeight)
        NSColor(calibratedRed: 0.12, green: 0.42, blue: 0.78, alpha: 1.0).setStroke()
        let border = NSBezierPath(roundedRect: destination.insetBy(dx: -2, dy: -2), xRadius: 4, yRadius: 4)
        border.lineWidth = 2
        border.stroke()
        image.draw(in: destination, from: .zero, operation: .copy, fraction: 1.0)
        drawText(item.2, top: top + imageHeight + 8, left: left, width: imageWidth, font: .systemFont(ofSize: 16), color: NSColor(calibratedWhite: 0.72, alpha: 1.0), canvasHeight: CGFloat(canvasHeight))
    }

    NSGraphicsContext.restoreGraphicsState()
    guard let data = bitmap.representation(using: .png, properties: [:]) else {
        throw MediaError.outputCreationFailed(outputPath)
    }
    try data.write(to: URL(fileURLWithPath: outputPath))
}

func writeGif(frameDirectory: String, outputPath: String) throws {
    let fileManager = FileManager.default
    let framePaths = try fileManager.contentsOfDirectory(atPath: frameDirectory)
        .filter { $0.hasSuffix(".png") }
        .sorted()
        .map { frameDirectory + "/" + $0 }
    guard !framePaths.isEmpty else {
        throw MediaError.noFrames(frameDirectory)
    }

    let outputURL = URL(fileURLWithPath: outputPath) as CFURL
    guard let destination = CGImageDestinationCreateWithURL(outputURL, UTType.gif.identifier as CFString, framePaths.count, nil) else {
        throw MediaError.outputCreationFailed(outputPath)
    }

    let gifProperties: [CFString: Any] = [
        kCGImagePropertyGIFDictionary: [kCGImagePropertyGIFLoopCount: 0]
    ]
    CGImageDestinationSetProperties(destination, gifProperties as CFDictionary)

    let frameProperties: [CFString: Any] = [
        kCGImagePropertyGIFDictionary: [
            kCGImagePropertyGIFDelayTime: 1.0 / 12.0,
            kCGImagePropertyGIFUnclampedDelayTime: 1.0 / 12.0
        ]
    ]

    for framePath in framePaths {
        let frameURL = URL(fileURLWithPath: framePath) as CFURL
        guard let source = CGImageSourceCreateWithURL(frameURL, nil),
              let image = CGImageSourceCreateImageAtIndex(source, 0, nil) else {
            throw MediaError.imageLoadFailed(framePath)
        }
        CGImageDestinationAddImage(destination, image, frameProperties as CFDictionary)
    }

    guard CGImageDestinationFinalize(destination) else {
        throw MediaError.outputCreationFailed(outputPath)
    }
}

do {
    let arguments = CommandLine.arguments
    guard arguments.count == 4 else {
        throw MediaError.invalidArguments
    }

    switch arguments[1] {
    case "comparison":
        try writeComparison(repositoryRoot: arguments[2], outputPath: arguments[3])
    case "gif":
        try writeGif(frameDirectory: arguments[2], outputPath: arguments[3])
    default:
        throw MediaError.invalidArguments
    }
} catch {
    FileHandle.standardError.write("Demo media generation failed: \(error)\n".data(using: .utf8)!)
    exit(1)
}
