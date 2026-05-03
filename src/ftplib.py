import os
from ftplib import FTP

def ftp_klasor_indir(ftp, uzaktaki_klasor, yerel_klasor):
    # Yerel klasörü oluştur
    if not os.path.exists(yerel_klasor):
        os.makedirs(yerel_klasor)

    ftp.cwd(uzaktaki_klasor)
    dosya_listesi = ftp.nlst()

    for dosya_adi in dosya_listesi:
        yerel_yol = os.path.join(yerel_klasor, dosya_adi)
        
        try:
            # Önce dosya mı yoksa alt klasör mü diye kontrol edilir
            ftp.cwd(dosya_adi)
            # Eğer hata vermezse bu bir klasördür, fonksiyon özyinelemeli (recursive) olarak tekrar çağrılır
            ftp_klasor_indir(ftp, dosya_adi, yerel_yol)
            ftp.cwd("..")
        except:
            # Klasör değilse dosyayı indir
            with open(yerel_yol, 'wb') as yerel_dosya:
                ftp.retrbinary(f"RETR {dosya_adi}", yerel_dosya.write)

# Kullanımı:
ftp = FTP('ftp.ornek-sunucu.com')
ftp.login('kullanici_adi', 'sifre')

ftp_klasor_indir(ftp, '/uzaktaki/klasor', 'C:/Bilgisayar/Kayit/Yeri')

ftp.quit()
