<?php

namespace DocIntegration;

use DocIntegration\Integration\IntegrationInterface;

class Integration
{
    private IntegrationInterface $integration;

    public function __construct(IntegrationInterface $integration)
    {
        $this->integration = $integration;
    }

    public function run(): void
    {
        $this->integration->execute();
    }
}