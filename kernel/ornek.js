'use strict';

const { kernelOlustur, KernelDurum } = require('./index');

async function ornekler() {
  // ─── 1. Basit kullanım ────────────────────────────────────────
  const kernel = kernelOlustur({
    zaman_asimi: 5000,
    hata_ayiklama: true,
  });

  console.log('Kernel durumu:', kernel.durumAl()); // 'hazir'

  // ─── 2. Fonksiyon çalıştırma ──────────────────────────────────
  const sonuc1 = await kernel.calistir(
    (baglam) => baglam.sayi * 2,
    { sayi: 21 }
  );
  console.log('Sonuç:', sonuc1.sonuc); // 42

  // ─── 3. Kod dizisi çalıştırma ─────────────────────────────────
  const sonuc2 = await kernel.calistir(
    `return isim.toUpperCase() + ' merhaba!'`,
    { isim: 'dünya' }
  );
  console.log('Sonuç:', sonuc2.sonuc); // 'DÜNYA merhaba!'

  // ─── 4. Hata yakalama ─────────────────────────────────────────
  const sonuc3 = await kernel.calistir(() => {
    throw new Error('Bir şeyler ters gitti');
  });
  console.log('Başarılı mı?', sonuc3.basarili); // false
  console.log('Hata:', sonuc3.hata);

  // ─── 5. Olay dinleyicisi ──────────────────────────────────────
  const abonelikten_cik = kernel.dinle('gorev:tamamlandi', ({ gorev_id, sure_ms }) => {
    console.log(`Görev ${gorev_id} tamamlandı (${sure_ms}ms)`);
  });

  await kernel.calistir(() => 'olay testi');
  abonelikten_cik(); // dinleyiciyi kaldır

  // ─── 6. Aktif görevleri listeleme ────────────────────────────
  const uzun_gorev = kernel.calistir(() =>
    new Promise(coz => setTimeout(() => coz('bitti'), 2000))
  );

  console.log('Aktif görevler:', kernel.aktifGorevler());
  await uzun_gorev;

  // ─── 7. Güvenli kapatma ───────────────────────────────────────
  await kernel.kapat();
  console.log('Kernel durumu:', kernel.durumAl()); // 'kapatildi'
}

ornekler().catch(console.error);