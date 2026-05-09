import gdown

# Klasörün herkese açık paylaşım linki
url = 'https://drive.google.com/drive/folders/KLASOR_ID_BURAYA_GELECEK'

# Klasörü içerikleriyle birlikte indirir ve zip olarak kaydeder
gdown.download_folder(url, quiet=False, use_cookies=False)
