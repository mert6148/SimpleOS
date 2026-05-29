#!/usr/bin/env php
<?php

declare(strict_types=1);

use Google\Documentation\DocumentationManager;

$autoload = __DIR__ . '/../vendor/autoload.php';
if (file_exists($autoload)) {
    require_once $autoload;
} else {
    require_once __DIR__ . '/../src/DocumentationInterface.php';
    require_once __DIR__ . '/../src/Documentation.php';
    require_once __DIR__ . '/../src/MarkdownRenderer.php';
    require_once __DIR__ . '/../src/DocumentationManager.php';
}

$docsPath = __DIR__ . '/../docs';
$outputPath = __DIR__ . '/../public';
$manager = new DocumentationManager($docsPath);
$manager->generateStaticSite($outputPath);

echo sprintf("Documentation generated: %s/index.html\n", realpath($outputPath) ?: $outputPath);
