<?php

declare(strict_types=1);

namespace SimpleOS\Docs\Log;

/**
 * Tek bir log kaydını temsil eden değişmez değer nesnesi.
 */
final class LogKaydi
{
    public readonly string $id;
    public readonly \DateTimeImmutable $zaman;

    /**
     * @param LogSeviye             $seviye     Log seviyesi
     * @param string                $mesaj      Ana mesaj
     * @param string                $kategori   Kaynak kategori (ör. "docs.footer")
     * @param array<string, mixed>  $baglam     Ek bağlam verisi
     * @param string|null           $dosya      Değişiklik yapılan dosya yolu
     * @param string|null           $yazar      İşlemi yapan kişi
     * @param string|null           $surum      Etkilenen sürüm numarası
     */
    public function __construct(
        public readonly LogSeviye $seviye,
        public readonly string    $mesaj,
        public readonly string    $kategori   = 'genel',
        public readonly array     $baglam     = [],
        public readonly ?string   $dosya      = null,
        public readonly ?string   $yazar      = null,
        public readonly ?string   $surum      = null,
        ?\DateTimeImmutable       $zaman      = null,
        ?string                   $id         = null,
    ) {
        $this->zaman = $zaman ?? new \DateTimeImmutable();
        $this->id    = $id    ?? $this->idUret();
    }

    /** Kayıttan ham dizi oluşturur (serileştirme / depolama için) */
    public function diziOlarakAl(): array
    {
        return [
            'id'       => $this->id,
            'zaman'    => $this->zaman->format(\DateTimeInterface::RFC3339_EXTENDED),
            'seviye'   => $this->seviye->etiketi(),
            'seviye_no'=> $this->seviye->value,
            'kategori' => $this->kategori,
            'mesaj'    => $this->mesaj,
            'dosya'    => $this->dosya,
            'yazar'    => $this->yazar,
            'surum'    => $this->surum,
            'baglam'   => $this->baglam,
        ];
    }

    /** Düz metin formatında tek satır döner */
    public function metinSatiri(bool $renkli = false): string
    {
        $zaman    = $this->zaman->format('Y-m-d H:i:s.v');
        $seviye   = str_pad($this->seviye->etiketi(), 7);
        $kategori = str_pad('[' . $this->kategori . ']', 20);
        $ek       = [];

        if ($this->dosya)  $ek[] = 'dosya=' . $this->dosya;
        if ($this->yazar)  $ek[] = 'yazar=' . $this->yazar;
        if ($this->surum)  $ek[] = 'sürüm=' . $this->surum;
        if (!empty($this->baglam)) {
            $ek[] = 'bağlam=' . json_encode($this->baglam, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
        }

        $satir = sprintf(
            '[%s] [%s] %s %s%s',
            $zaman,
            $seviye,
            $kategori,
            $this->mesaj,
            $ek ? ' | ' . implode(' | ', $ek) : ''
        );

        if ($renkli) {
            $sifirla = "\033[0m";
            $renk    = $this->seviye->rengi();
            $satir   = $renk . $satir . $sifirla;
        }

        return $satir;
    }

    // ─── Fabrika Yardımcıları ─────────────────────────────

    public static function bilgi(string $mesaj, string $kategori = 'genel', array $baglam = []): self
    {
        return new self(LogSeviye::BILGI, $mesaj, $kategori, $baglam);
    }

    public static function hata(string $mesaj, string $kategori = 'genel', array $baglam = []): self
    {
        return new self(LogSeviye::HATA, $mesaj, $kategori, $baglam);
    }

    public static function uyari(string $mesaj, string $kategori = 'genel', array $baglam = []): self
    {
        return new self(LogSeviye::UYARI, $mesaj, $kategori, $baglam);
    }

    public static function debug(string $mesaj, string $kategori = 'genel', array $baglam = []): self
    {
        return new self(LogSeviye::DEBUG, $mesaj, $kategori, $baglam);
    }

    public static function diziden(array $veri): self
    {
        return new self(
            seviye:   LogSeviye::from((int) ($veri['seviye_no'] ?? 5)),
            mesaj:    $veri['mesaj']    ?? '',
            kategori: $veri['kategori'] ?? 'genel',
            baglam:   $veri['baglam']   ?? [],
            dosya:    $veri['dosya']    ?? null,
            yazar:    $veri['yazar']    ?? null,
            surum:    $veri['surum']    ?? null,
            zaman:    isset($veri['zaman'])
                ? new \DateTimeImmutable($veri['zaman'])
                : null,
            id:       $veri['id'] ?? null,
        );
    }

    // ─── Özel ─────────────────────────────────────────────

    private function idUret(): string
    {
        return sprintf(
            '%s-%s',
            $this->zaman->format('YmdHisv'),
            substr(bin2hex(random_bytes(4)), 0, 8)
        );
    }
}
