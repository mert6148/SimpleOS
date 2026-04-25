<?php

namespace Cache\Docs;

// Varsayımsal arayüz (Interface tanımlı değilse hata vermemesi için basitçe ekledik)
interface CacheInterface {
    public function get($key, $default = null);
    public function set($key, $value, $ttl = null);
    public function delete($key);
    public function clear();
    public function has($key);
}

class DocsCache implements CacheInterface
{
    // Verileri saklamak için statik bir dizi
    private static $storage = [];

    public function get($key, $default = null)
    {
        return $this->has($key) ? self::$storage[$key] : $default;
    }

    public function set($key, $value, $ttl = null)
    {
        // SimpleOS: Burada TTL (zaman aşımı) mantığı eklenebilir
        self::$storage[$key] = $value;
        return true;
    }

    public function delete($key)
    {
        if ($this->has($key)) {
            unset(self::$storage[$key]);
            return true;
        }
        return false;
    }

    public function clear()
    {
        self::$storage = [];
        return true;
    }

    public function has($key)
    {
        return isset(self::$storage[$key]);
    }
}

class DocsCacheItem
{
    private $key;
    private $value;
    private $ttl;

    public function __construct($key, $value, $ttl = null)
    {
        $this->key = $key;
        $this->value = $value;
        $this->ttl = $ttl;
    }

    public function getKey()   { return $this->key; }
    public function getValue() { return $this->value; }
    public function getTtl()   { return $this->ttl; }
}

class DocsCachePool
{
    private $items = [];

    public function getItem($key)
    {
        return $this->items[$key] ?? null;
    }

    public function save(DocsCacheItem $item)
    {
        $this->items[$item->getKey()] = $item;
        return true;
    }

    public function deleteItem($key)
    {
        unset($this->items[$key]);
    }

    public function clear()
    {
        $this->items = [];
    }
}

class DocsCacheException extends \Exception
{
    private $cacheKey;

    public function __construct($message = "", $code = 0, $cacheKey = null, \Throwable $previous = null)
    {
        $this->cacheKey = $cacheKey;
        parent::__construct($message, $code, $previous);
    }

    public function getCacheKey()
    {
        return $this->cacheKey;
    }
}
