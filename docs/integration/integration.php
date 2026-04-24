<?php

namespace DocIntegration;

use DocIntegration\Integration\IntegrationInterface;

class Integration
{
    private IntegrationInterface $integration;

    public function __construct(IntegrationInterface $integration)
    {
        try {
            $this->integration = $integration;
        } catch (\Exception $e) {
            throw new \RuntimeException('Failed to initialize integration: ' . $e->getMessage());
        } else {
            echo "Integration initialized successfully.\n";
        }
    }

    public function run(): void
    {
        foreach ($this->integration->getEndpoints() as $endpoint) {
            $response = $this->integration->callEndpoint($endpoint);
            echo "Response from {$endpoint}: {$response}\n";
        }
    }

    public function getIntegration(): IntegrationInterface
    {
        if (!$this->integration) {
            throw new \RuntimeException('Integration not set.');
        }
        return $this->integration;
    }

    public function setIntegration(IntegrationInterface $integration): void
    {
        try {
            $this->integration = $integration;
        } catch (\Exception $e) {
            throw new \RuntimeException('Failed to set integration: ' . $e->getMessage());
        } else {
            echo "Integration set successfully.\n";
        }
    }
}

class getIntegration
{
    public function __invoke(Integration $integration): IntegrationInterface
    {
        try {
            return $integration->getIntegration();
        } catch (\Exception $e) {
            throw new \RuntimeException('Failed to get integration: ' . $e->getMessage());
        }
    }
}
