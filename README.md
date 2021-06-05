# Raspberry-ile-Otonom-Surus
Raspberry Pi ve Arduino UNO İle Otonom Sürüş Yapan Araba Projesi.

# İçerik
- [Özet](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus#%C3%B6zet)
- [Donanım](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus#donan%C4%B1m)
- [Sistem Şeması ve Bağlantılar](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus#sistem-%C5%9Femas%C4%B1)
- [Şerit Takibi](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus#%C5%9Ferit-takibi)
- [Nesne ve DUR İşareti Algılama](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus#nesne-ve-dur-i%CC%87%C5%9Fareti-alg%C4%B1lama)
- [Kodlama](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus#kodlama)

## Özet
Projemiz,8 Mp'lik bir Raspberry Pi Kamerası ile gerçek zamanlı görüntü işleme yöntemini kullanarak otonom sürüş gerçekleştirmek üzere planlanmıştır.Araç bu otonom sürüşte:
 
- Şerit Takibi
- Dur İşareti Algılama
- Nesne ve DUR İşareti Algılama
- Yol Sonu Algılama ve U Dönüşü    

gibi belirli olayları görüntü işleme sayesinde otonom olarak gerçekleştirebilme özelliğini sahiptir.

[![Youtube](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus/blob/main/Raspberry%20Pi%20ile%20Otonom%20S%C3%BCr%C3%BC%C5%9F/G%C3%B6rseller/Youtube1.jpg)](https://www.youtube.com/watch?v=oR7y1ipzKCg)






## Donanım
![Donanım](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus/blob/main/Raspberry%20Pi%20ile%20Otonom%20S%C3%BCr%C3%BC%C5%9F/G%C3%B6rseller/Donan%C4%B1m.png)

## Sistem Şeması
![Sistem Şeması](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus/blob/main/Raspberry%20Pi%20ile%20Otonom%20S%C3%BCr%C3%BC%C5%9F/G%C3%B6rseller/Devre%20%C5%9Eemas%C4%B1.png)

Raspberry Pi ve Arduino UNO arasındaki haberleşme için ise WiringPi kütüphanesi yardımıyla 25,24,23,21 numaralı bağlantıları kullanıyoruz.

![Haberleşme](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus/blob/main/Raspberry%20Pi%20ile%20Otonom%20S%C3%BCr%C3%BC%C5%9F/G%C3%B6rseller/Raspberry%20Pin%20Diagram.png)

## Şerit Takibi

Şerit Takibi İçin Şu Aşamaları Kullanıyoruz:
- 1.ROI oluşturma: Bu aşamada kamera görüntüsünde çizgilere paralel bir ilgi bölgesi oluşturuyoruz.
- 2.Perspektif Dönüşümü: İlgi bölgesine aldığımız alanı kuş bakışı olarak ele alıyoruz.
- 3.Threshold: Görüntü eşikleme yöntemi ile pikselleri, verilen eşik değerine göre siyah ya da beyaz olarak güncelliyoruz.
- 4.Canny Kenar Algılama: Görüntü üzerindeki kenar tespiti ile perspektif haldeki görüntümüzde yol kenarlarını belirliyoruz.
- 5.Histogram ve Vektörler: Kameranın piksel genişlik ve uzunluğunu dikkate alarak sol ve sağ çizgilerin arasındaki mesafe hesaplanır.Yolun Orta Noktası bulunur.

Sonuç olarak Kodlama Ekranını Çalıştırdığımızda Aşağıdaki Ekranlar Üzerinden Otonom Sürüşün Takibini Yapabiliriz.

![Kodlama Ekranı](https://github.com/ataberktekir/Raspberry-ile-Otonom-Surus/blob/main/Raspberry%20Pi%20ile%20Otonom%20S%C3%BCr%C3%BC%C5%9F/G%C3%B6rseller/Kod%20Ekran%C4%B1.png)

## Nesne ve DUR İşareti Algılama

Nesne Algılama için "Cascade Classifier Machine Learning" algoritmasını kullandık.
Modeli farklı açılardan 45 adet,yine aynı şekilde farklı açılardan 299 adet negatif fotoğraflar ile eğittik.Eğitim sonucunda ortaya çıkan cascade dosyasını OpenCV kütüphanesi olarak kullandık.

## Kodlama

Projemizde Programlama dili olarak C++ kullandık.OpenCV kütüphanesini kullanarak görüntüyü anlık olarak alma,kaydetme ve görüntü işleme algoritmalarını elde ettik.
Arduino İno programı ile motorların binary kodlamasını oluşturduk ve Raspberry-Arduino arasında Master-Slave cihaz sistemini kurduk.
Bu sistem sayesinde anlık olarak gelen görüntü işleme sonuçlarındaki binary değerler gecikmesiz olarak arduino'ya ulaştı ve motorlar kodlamada oluşturulan düzende çalışabildi.
