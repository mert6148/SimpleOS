import requests

def klasor_indir(url, kayit_yeri):
    print("İndiriliyor...")
    yanit = requests.get(url, stream=True)
    
    with open(kayit_yeri, 'wb') as dosya:
        for veri in yanit.iter_content(chunk_size=1024):
            if veri:
                dosya.write(veri)
    print("İndirme tamamlandı!")

# Kullanımı:
url = "https://ornek-site.com/dosyalar/arsiv.zip"
kayit_yeri = "indirilen_arsiv.zip"
klasor_indir(url, kayit_yeri)
