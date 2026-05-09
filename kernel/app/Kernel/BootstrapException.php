<?php

namespace Kernel\Exceptions;

class BootstrapException extends KernelException
{
    private string $bootstrapSinifi;

    public function __construct(string $bootstrapSinifi, string $mesaj, int $kod = 0, ?\Throwable $onceki = null)
    {
        $this->bootstrapSinifi = $bootstrapSinifi;
        parent::__construct("[{$bootstrapSinifi}] Bootstrap hatası: {$mesaj}", $kod, $onceki);
    }

    public function getBootstrapSinifi(): string
    {
        return $this->bootstrapSinifi;
    }
}
