<?php

declare(strict_types=1);

spl_autoload_register(static function (string $class): void {
    // App\Kernel namespace
    $kernelPrefix = 'App\\Kernel\\';
    if (str_starts_with($class, $kernelPrefix)) {
        $relativeClass = substr($class, strlen($kernelPrefix));
        $file = __DIR__ . '/App/Kernel/' . str_replace('\\', '/', $relativeClass) . '.php';
        if (file_exists($file)) {
            require $file;
        }
        return;
    }
    
    // App\Script\SysProtocol namespace
    $sysProtocolPrefix = 'App\\Script\\SysProtocol\\';
    if (str_starts_with($class, $sysProtocolPrefix)) {
        $relativeClass = substr($class, strlen($sysProtocolPrefix));
        $file = __DIR__ . '/App/Script/SysProtocol/' . str_replace('\\', '/', $relativeClass) . '.php';
        if (file_exists($file)) {
            require $file;
        }
        return;
    }
    
    // App\Script\Package namespace
    $packagePrefix = 'App\\Script\\Package\\';
    if (str_starts_with($class, $packagePrefix)) {
        $relativeClass = substr($class, strlen($packagePrefix));
        $file = __DIR__ . '/App/Script/Package/' . str_replace('\\', '/', $relativeClass) . '.php';
        if (file_exists($file)) {
            require $file;
        }
    }
});
