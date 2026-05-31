<?php

namespace App;

use App\Script\Package\PackageApplication;

class App
{
    public static function run(array $args): void
    {
        if (count($args) < 2) {
            self::printHelp();
            return;
        }

        $command = $args[1];

        switch ($command) {
            case 'package':
                PackageApplication::run(array_slice($args, 1));
                break;
            case 'help':
            case '--help':
            case '-h':
                self::printHelp();
                break;
            default:
                echo "Unknown command: $command\n";
                self::printHelp();
                break;
        }
    }

    private static function printHelp(): void
    {
        echo "Usage: php script.php <command> [options]\n";
        echo "\n";
        echo "Commands:\n";
        echo "  package      Run the package manager for SimpleOS system and application packages\n";
        echo "  help         Show this help message\n";
        echo "\n";
        echo "Package command examples:\n";
        echo "  php script.php package list\n";
        echo "  php script.php package install kernel-headers\n";
        echo "  php script.php package info core-utils\n";
        echo "  php script.php package app list\n";
        echo "  php script.php package app install text-editor\n";
        echo "  php script.php package app info file-manager\n";
        echo "  php script.php package app run text-editor -- notes.txt\n";
        echo "  php script.php package app remove process-viewer\n";
        echo "  php script.php package version\n";
        echo "  php script.php package prompt list\n";
        echo "  php script.php package prompt info kernel-init-prompt\n";
        echo "  php script.php package prompt create custom-prompt --mode=system\n";
        echo "\n";
        echo "Kernel and system package commands:\n";
        echo "  php script.php package kernel\n";
        echo "  php script.php package kernel-init\n";
        echo "  php script.php package kernel-status\n";
        echo "  php script.php package protocol\n";
        echo "  php script.php package network\n";
        echo "  php script.php package validate-ip 192.168.1.1\n";
        echo "  php script.php package endpoint 127.0.0.1 8080\n";
        echo "  php script.php package system-check\n";
    }

    public static function main(): void
    {
        chdir(__DIR__ . '/..');
        self::run($_SERVER['argv']);
    }
}

class Main
{
    public static function main(): void
    {
        App::main();
    }
}