<?php

namespace Kernel\ServiceContainer;

use Kernel\Exceptions\KernelException;

/**
 * Basit Dependency Injection konteyneri.
 * Servisleri singleton veya fabrika olarak kaydeder ve çözümler.
 */
class Container
{
    /** @var array<string, callable> Fabrika tanımları */
    private array $baglamalar = [];

    /** @var array<string, object> Singleton önbelleği */
    private array $ornekler = [];

    /** @var array<string, bool> Singleton işareti */
    private array $tektonlar = [];

    /**
     * Servisi fabrika fonksiyonu ile kaydeder.
     *
     * @param string   $soyut   Servis adı veya arayüz
     * @param callable $fabrika Nesne oluşturan fonksiyon
     * @param bool     $tektone Her çağrıda aynı örneği dönüp döndürmeyeceği
     */
    public function bagla(string $soyut, callable $fabrika, bool $tektone = false): void
    {
        $this->baglamalar[$soyut] = $fabrika;
        $this->tektonlar[$soyut]  = $tektone;
        // Daha önce önbelleklenmişse temizle
        unset($this->ornekler[$soyut]);
    }

    /**
     * Servisi singleton olarak kaydeder.
     */
    public function tektone(string $soyut, callable $fabrika): void
    {
        $this->bagla($soyut, $fabrika, true);
    }

    /**
     * Hazır nesneyi doğrudan singleton olarak kaydeder.
     */
    public function ornek(string $soyut, object $nesne): void
    {
        $this->ornekler[$soyut] = $nesne;
        $this->tektonlar[$soyut] = true;
        $this->baglamalar[$soyut] = fn() => $nesne;
    }

    /**
     * Servisi çözümler ve döner.
     *
     * @throws KernelException Servis kayıtlı değilse
     */
    public function coz(string $soyut): mixed
    {
        if (!isset($this->baglamalar[$soyut])) {
            throw new KernelException("Servis konteynerde bulunamadı: '{$soyut}'");
        }

        // Singleton ve önbelleklenmişse direkt dön
        if ($this->tektonlar[$soyut] && isset($this->ornekler[$soyut])) {
            return $this->ornekler[$soyut];
        }

        $sonuc = ($this->baglamalar[$soyut])($this);

        if ($this->tektonlar[$soyut]) {
            $this->ornekler[$soyut] = $sonuc;
        }

        return $sonuc;
    }

    /**
     * Servisin kayıtlı olup olmadığını kontrol eder.
     */
    public function var_mi(string $soyut): bool
    {
        return isset($this->baglamalar[$soyut]);
    }

    /**
     * Tüm kayıtlı servis adlarını döner.
     *
     * @return string[]
     */
    public function kayitlilar(): array
    {
        return array_keys($this->baglamalar);
    }
}