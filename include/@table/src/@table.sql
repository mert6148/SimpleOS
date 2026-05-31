-- =============================================================================
--  @table.sql — SQL Sistem Tabloları Komut Bilgisi ve Kod Dizini
--  Proje  : SimpleOS
--  Konum  : include/@table/src/@table.sql
--  Yazar  : SimpleOS Geliştirme Ekibi
--  Tarih  : 2025
-- =============================================================================


-- =============================================================================
-- [1] DDL — VERİ TANIMI DİLİ (Data Definition Language)
-- =============================================================================

-- -----------------------------------------------------------------------------
-- 1.1  Tablo Oluşturma
--      Yeni bir tablo tanımlar.
-- -----------------------------------------------------------------------------
CREATE TABLE kullanicilar (
    id          INT             PRIMARY KEY AUTO_INCREMENT,
    ad          VARCHAR(100)    NOT NULL,
    eposta      VARCHAR(255)    UNIQUE,
    olusturma   TIMESTAMP       DEFAULT CURRENT_TIMESTAMP
);

-- -----------------------------------------------------------------------------
-- 1.2  Tablo Silme
--      Tabloyu ve tüm verilerini kalıcı siler.
-- -----------------------------------------------------------------------------
DROP TABLE IF EXISTS kullanicilar;

-- -----------------------------------------------------------------------------
-- 1.3  Sütun Ekleme — ALTER ADD
--      Mevcut tabloya yeni sütun ekler.
-- -----------------------------------------------------------------------------
ALTER TABLE kullanicilar
    ADD COLUMN telefon VARCHAR(20) AFTER eposta;

-- -----------------------------------------------------------------------------
-- 1.4  Sütun Değiştirme — ALTER MODIFY
--      Sütun tipini veya kısıtını değiştirir.
-- -----------------------------------------------------------------------------
ALTER TABLE kullanicilar
    MODIFY COLUMN eposta VARCHAR(320) NOT NULL;

-- -----------------------------------------------------------------------------
-- 1.5  Sütun Yeniden Adlandırma
--      Sütun adını değiştirir (MySQL 8.0+).
-- -----------------------------------------------------------------------------
ALTER TABLE kullanicilar
    RENAME COLUMN eposta TO email;

-- -----------------------------------------------------------------------------
-- 1.6  İndeks Oluşturma
--      Sorgu hızını artırmak için indeks tanımlar.
-- -----------------------------------------------------------------------------
CREATE INDEX idx_kullanici_email
    ON kullanicilar (eposta);

CREATE UNIQUE INDEX idx_uniq_email
    ON kullanicilar (eposta);

-- -----------------------------------------------------------------------------
-- 1.7  Foreign Key Kısıtı
--      Tablolar arası referans bütünlüğü sağlar.
-- -----------------------------------------------------------------------------
ALTER TABLE siparisler
    ADD CONSTRAINT fk_kullanici
    FOREIGN KEY (kullanici_id)
    REFERENCES kullanicilar(id)
    ON DELETE CASCADE
    ON UPDATE RESTRICT;

-- -----------------------------------------------------------------------------
-- 1.8  View (Görünüm) Oluşturma
--      Kayıtlı bir SELECT sorgusu tanımlar.
-- -----------------------------------------------------------------------------
CREATE OR REPLACE VIEW aktif_kullanicilar AS
    SELECT id, ad, eposta
    FROM   kullanicilar
    WHERE  aktif = 1;


-- =============================================================================
-- [2] DML — VERİ İŞLEME DİLİ (Data Manipulation Language)
-- =============================================================================

-- -----------------------------------------------------------------------------
-- 2.1  Veri Ekleme — INSERT
--      Tabloya bir veya birden fazla satır ekler.
-- -----------------------------------------------------------------------------
INSERT INTO kullanicilar (ad, eposta)
VALUES ('Ahmet Yılmaz', 'ahmet@ornek.com');

INSERT INTO kullanicilar (ad, eposta) VALUES
    ('Mehmet Kaya',  'mehmet@ornek.com'),
    ('Ayşe Demir',   'ayse@ornek.com');

-- -----------------------------------------------------------------------------
-- 2.2  Veri Seçme — SELECT
--      Tablodan veri okur; filtre, sıralama ve sayfalama.
-- -----------------------------------------------------------------------------
SELECT id, ad, eposta
FROM   kullanicilar
WHERE  aktif = 1
  AND  olusturma >= '2024-01-01'
ORDER  BY ad ASC
LIMIT  20 OFFSET 40;

-- -----------------------------------------------------------------------------
-- 2.3  Veri Güncelleme — UPDATE
--      Mevcut satırların değerini değiştirir.
-- -----------------------------------------------------------------------------
UPDATE kullanicilar
SET    telefon    = '05551234567',
       guncelleme = NOW()
WHERE  id = 42;

-- -----------------------------------------------------------------------------
-- 2.4  Veri Silme — DELETE
--      Koşulla eşleşen satırları siler.
-- -----------------------------------------------------------------------------
DELETE FROM kullanicilar
WHERE  aktif = 0
  AND  olusturma < DATE_SUB(NOW(), INTERVAL 1 YEAR);

-- -----------------------------------------------------------------------------
-- 2.5  Upsert — INSERT … ON DUPLICATE KEY
--      Varsa günceller, yoksa ekler (MySQL).
-- -----------------------------------------------------------------------------
INSERT INTO ayarlar (kullanici_id, anahtar, deger)
VALUES (42, 'tema', 'karanlik')
ON DUPLICATE KEY UPDATE
    deger      = VALUES(deger),
    guncelleme = NOW();

-- -----------------------------------------------------------------------------
-- 2.6  TRUNCATE — Tüm Satırları Sil
--      Tabloyu hızlıca boşaltır; AUTO_INCREMENT sıfırlar.
-- -----------------------------------------------------------------------------
TRUNCATE TABLE oturum_gecmisi;


-- =============================================================================
-- [3] DCL — VERİ KONTROL DİLİ (Data Control Language)
-- =============================================================================

-- -----------------------------------------------------------------------------
-- 3.1  Kullanıcı Yetkisi Verme — GRANT
--      Kullanıcıya veritabanı/tablo erişimi tanır.
-- -----------------------------------------------------------------------------
GRANT SELECT, INSERT, UPDATE
    ON uygulama_db.*
    TO 'api_kullanici'@'%'
    IDENTIFIED BY 'guclu_sifre';

FLUSH PRIVILEGES;

-- -----------------------------------------------------------------------------
-- 3.2  Yetki Geri Alma — REVOKE
--      Kullanıcıdan belirli izinleri alır.
-- -----------------------------------------------------------------------------
REVOKE DELETE
    ON uygulama_db.*
    FROM 'api_kullanici'@'%';

-- -----------------------------------------------------------------------------
-- 3.3  Transaction — Commit / Rollback
--      İşlemleri atomik hale getirir.
-- -----------------------------------------------------------------------------
START TRANSACTION;

UPDATE hesaplar SET bakiye = bakiye - 500 WHERE id = 1;
UPDATE hesaplar SET bakiye = bakiye + 500 WHERE id = 2;

COMMIT;
-- Hata durumunda: ROLLBACK;


-- =============================================================================
-- [4] SİSTEM TABLOLARI — information_schema Sorguları
-- =============================================================================

-- -----------------------------------------------------------------------------
-- 4.1  Tüm Tabloları Listele
--      Veritabanındaki tabloları ve boyutlarını gösterir.
-- -----------------------------------------------------------------------------
SELECT
    table_name                              AS tablo_adi,
    table_rows                              AS satir_sayisi,
    ROUND(data_length  / 1024 / 1024, 2)   AS veri_mb,
    ROUND(index_length / 1024 / 1024, 2)   AS indeks_mb
FROM  information_schema.tables
WHERE table_schema = DATABASE()
ORDER BY data_length DESC;

-- -----------------------------------------------------------------------------
-- 4.2  Sütun Bilgilerini Getir
--      Tablonun tüm sütun meta verisini listeler.
-- -----------------------------------------------------------------------------
SELECT
    column_name     AS sutun,
    column_type     AS tip,
    is_nullable     AS null_mu,
    column_default  AS varsayilan,
    column_key      AS anahtar,
    extra
FROM  information_schema.columns
WHERE table_schema = DATABASE()
  AND table_name   = 'kullanicilar'
ORDER BY ordinal_position;

-- -----------------------------------------------------------------------------
-- 4.3  İndeksleri Listele
--      Tablodaki tüm indeksleri ve kapsanan sütunları gösterir.
-- -----------------------------------------------------------------------------
SELECT
    index_name   AS indeks_adi,
    column_name  AS sutun,
    non_unique   AS tekrar_izinli,
    seq_in_index AS siralama
FROM  information_schema.statistics
WHERE table_schema = DATABASE()
  AND table_name   = 'kullanicilar';

-- -----------------------------------------------------------------------------
-- 4.4  Foreign Key İlişkilerini Göster
--      Tablonun tüm FK kısıtlarını listeler.
-- -----------------------------------------------------------------------------
SELECT
    kcu.constraint_name,
    kcu.column_name                 AS kaynak_sutun,
    kcu.referenced_table_name       AS hedef_tablo,
    kcu.referenced_column_name      AS hedef_sutun,
    rc.delete_rule,
    rc.update_rule
FROM  information_schema.key_column_usage       kcu
JOIN  information_schema.referential_constraints rc
    ON rc.constraint_name = kcu.constraint_name
WHERE kcu.table_schema = DATABASE()
  AND kcu.table_name   = 'siparisler';

-- -----------------------------------------------------------------------------
-- 4.5  Çalışan Sorguları Gör — PROCESSLIST
--      Aktif bağlantıları ve sorgularını gösterir.
-- -----------------------------------------------------------------------------
SELECT
    id,
    user,
    host,
    db,
    command,
    time,
    state,
    LEFT(info, 100) AS sorgu
FROM  information_schema.processlist
WHERE command != 'Sleep'
ORDER BY time DESC;


-- =============================================================================
-- [5] JOIN — TABLO BİRLEŞTİRME
-- =============================================================================

-- -----------------------------------------------------------------------------
-- 5.1  INNER JOIN
--      Her iki tabloda da eşleşen satırları getirir.
-- -----------------------------------------------------------------------------
SELECT
    k.ad,
    s.siparis_tarihi,
    s.toplam
FROM  kullanicilar k
INNER JOIN siparisler s ON s.kullanici_id = k.id
WHERE s.durum = 'tamamlandi';

-- -----------------------------------------------------------------------------
-- 5.2  LEFT JOIN
--      Sol tablodaki tüm satırları, eşleşme yoksa NULL ile getirir.
-- -----------------------------------------------------------------------------
SELECT
    k.ad,
    COUNT(s.id) AS siparis_sayisi
FROM  kullanicilar k
LEFT  JOIN siparisler s ON s.kullanici_id = k.id
GROUP BY k.id, k.ad
ORDER BY siparis_sayisi DESC;

-- -----------------------------------------------------------------------------
-- 5.3  Çoklu JOIN + Subquery
--      Birden fazla tablo birleştirme ve iç sorgu kullanımı.
-- -----------------------------------------------------------------------------
SELECT
    k.ad,
    u.ad            AS urun_adi,
    sd.adet,
    sd.birim_fiyat
FROM  siparisler     s
JOIN  kullanicilar   k  ON k.id  = s.kullanici_id
JOIN  siparis_detay  sd ON sd.siparis_id = s.id
JOIN  urunler        u  ON u.id  = sd.urun_id
WHERE s.id IN (
    SELECT id
    FROM   siparisler
    WHERE  toplam > 1000
);

-- -----------------------------------------------------------------------------
-- 5.4  SELF JOIN
--      Tablonun kendisiyle birleştirilmesi (hiyerarşi).
-- -----------------------------------------------------------------------------
SELECT
    c.id,
    c.ad    AS calisan,
    y.ad    AS yonetici
FROM  calisanlar c
LEFT  JOIN calisanlar y ON y.id = c.yonetici_id;


-- =============================================================================
-- [6] FONKSİYONLAR — Pencere / CTE / Saklı Yordam / Trigger
-- =============================================================================

-- -----------------------------------------------------------------------------
-- 6.1  Pencere Fonksiyonu — ROW_NUMBER
--      Her satıra sıralı numara atar (MySQL 8+, MSSQL, PostgreSQL).
-- -----------------------------------------------------------------------------
SELECT
    ad,
    departman,
    maas,
    ROW_NUMBER() OVER (
        PARTITION BY departman
        ORDER BY     maas DESC
    ) AS departman_sirasi
FROM calisanlar;

-- -----------------------------------------------------------------------------
-- 6.2  Pencere Fonksiyonu — LAG / LEAD
--      Önceki/sonraki satır değerine erişir.
-- -----------------------------------------------------------------------------
SELECT
    tarih,
    satis,
    LAG (satis, 1, 0) OVER (ORDER BY tarih) AS onceki_gun,
    LEAD(satis, 1, 0) OVER (ORDER BY tarih) AS sonraki_gun,
    satis - LAG(satis, 1, 0) OVER (ORDER BY tarih) AS degisim
FROM gunluk_satis;

-- -----------------------------------------------------------------------------
-- 6.3  CTE — WITH (Ortak Tablo İfadesi)
--      Okunabilir alt sorgu bloğu tanımlar.
-- -----------------------------------------------------------------------------
WITH aylik_satis AS (
    SELECT
        DATE_FORMAT(tarih, '%Y-%m')  AS ay,
        SUM(toplam)                  AS ciro
    FROM   siparisler
    WHERE  durum = 'tamamlandi'
    GROUP  BY ay
),
buyume AS (
    SELECT
        ay,
        ciro,
        LAG(ciro) OVER (ORDER BY ay)  AS gecen_ay,
        ROUND(
            100.0 * (ciro - LAG(ciro) OVER (ORDER BY ay))
            / NULLIF(LAG(ciro) OVER (ORDER BY ay), 0),
        2) AS buyume_pct
    FROM aylik_satis
)
SELECT * FROM buyume ORDER BY ay DESC;

-- -----------------------------------------------------------------------------
-- 6.4  Stored Procedure — Parametreli Saklı Yordam
--      Parametreli saklı yordam oluşturur ve çağırır.
-- -----------------------------------------------------------------------------
DELIMITER $$

CREATE PROCEDURE siparis_ozetle(IN p_kullanici_id INT)
BEGIN
    SELECT
        COUNT(*)            AS toplam_siparis,
        SUM(toplam)         AS toplam_tutar,
        MAX(siparis_tarihi) AS son_siparis
    FROM  siparisler
    WHERE kullanici_id = p_kullanici_id
      AND durum        = 'tamamlandi';
END$$

DELIMITER ;

-- Çağrı:
CALL siparis_ozetle(42);

-- -----------------------------------------------------------------------------
-- 6.5  Trigger — Otomatik Değişiklik Logu
--      Tablo değişikliklerinde otomatik kayıt tutar.
-- -----------------------------------------------------------------------------
CREATE TRIGGER trg_kullanici_guncelleme
AFTER UPDATE ON kullanicilar
FOR EACH ROW
BEGIN
    INSERT INTO degisiklik_logu
        (tablo_adi, kayit_id, eski_deger, yeni_deger, degistiren)
    VALUES
        (
            'kullanicilar',
            OLD.id,
            JSON_OBJECT('eposta', OLD.eposta),
            JSON_OBJECT('eposta', NEW.eposta),
            CURRENT_USER()
        );
END;


-- =============================================================================
-- EOF — @table.sql
-- =============================================================================
