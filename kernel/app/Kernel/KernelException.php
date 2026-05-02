<?php

namespace Kernel\Exceptions;

class KernelException extends \RuntimeException
{
    public function __construct(string $mesaj, int $kod = 0, ?\Throwable $onceki = null)
    {
        parent::__construct($mesaj, $kod, $onceki);
    }
}
