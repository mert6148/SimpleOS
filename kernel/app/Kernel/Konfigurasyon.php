<?php

namespace Kernel\Config;

use Kernel\Exceptions\KernelException;

/**
 * Konfigürasyon yöneticisi.
 * Noktalı-gösterim ile iç içe değerlere erişim sağlar: "veritabani.host"
 */
class Konfigurasyon
{
    /** @var array<string, mixed> Yüklü konfigürasyon verileri */
    private array $veri = [];

    /**
     * Konfigürasyon verisini belirli bir isim alanına yükler.
     *
     * @param string       $isimAlani "veritabani", "uygulama" vb.
     * @param array<mixed> $degerler  Konfigürasyon değerleri
     */
    public function yukle(string $isimAlani, array $degerler): void
    {
        $this->veri[$isimAlani] = array_merge(
            $this->veri[$isimAlani] ?? [],
            $degerler
        );
    }

    /**
     * Konfigürasyon değerini döner.
     * Noktalı-gösterim desteklenir: "veritabani.host"
     *
     * @param string $anahtar  Noktalı anahtar
     * @param mixed  $varsayilan Bulunamazsa dönecek değer
     */
    public function al(string $anahtar, mixed $varsayilan = null): mixed
    {
        $parcalar = explode('.', $anahtar);
        $mevcut   = $this->veri;

        foreach ($parcalar as $parca) {
            if (!is_array($mevcut) || !array_key_exists($parca, $mevcut)) {
                return $varsayilan;
            }
            $mevcut = $mevcut[$parca];
        }

        return $mevcut;
    }

    /**
     * Konfigürasyon değerini zorunlu olarak döner.
     * Bulunamazsa istisna fırlatır.
     *
     * @throws KernelException
     */
    public function zorunluAl(string $anahtar): mixed
    {
        $deger = $this->al($anahtar, '__BULUNAMADI__');

        if ($deger === '__BULUNAMADI__') {
            throw new KernelException("Zorunlu konfigürasyon bulunamadı: '{$anahtar}'");
        }

        return $deger;
    }

    /**
     * Konfigürasyon değerini ayarlar (runtime).
     */
    public function ayarla(string $anahtar, mixed $deger): void
    {
        $parcalar = explode('.', $anahtar);
        $hedef    = &$this->veri;

        foreach ($parcalar as $i => $parca) {
            if ($i === count($parcalar) - 1) {
                $hedef[$parca] = $deger;
            } else {
                if (!isset($hedef[$parca]) || !is_array($hedef[$parca])) {
                    $hedef[$parca] = [];
                }
                $hedef = &$hedef[$parca];
            }
        }
    }

    /**
     * Tüm konfigürasyon verisini döner.
     *
     * @return array<string, mixed>
     */
    public function tumVeri(): array
    {
        return $this->veri;
    }
}
