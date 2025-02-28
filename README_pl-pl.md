**[简体中文](./README.md) | Polish**

[![Badge](https://img.shields.io/badge/link-996.icu-%23FF4D5B.svg?style=flat-square)](https://996.icu/#/en_US)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg?style=flat-square)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/zhongyang219/TrafficMonitor/main.yml?branch=master&label=Release%20CI&logo=github&style=flat-square)](https://github.com/zhongyang219/TrafficMonitor/actions?query=workflow:"Release+CI")
[![GitHub release](https://img.shields.io/github/release/zhongyang219/TrafficMonitor.svg?style=flat-square)](https://github.com/zhongyang219/TrafficMonitor/releases/latest)

<a href="https://hellogithub.com/repository/5ef48af2b2794d4798b17d6539ec7305" target="_blank"><img src="https://api.hellogithub.com/v1/widgets/recommend.svg?rid=5ef48af2b2794d4798b17d6539ec7305&claim_uid=CeVqou2T1dIvfQP" alt="Featured｜HelloGitHub" style="width: 250px; height: 54px;" width="250" height="54" /></a>

# Wprowadzenie do TrafficMonitor

TrafficMonitor to oprogramowanie do monitorowania sieci z funkcją pływającego okna dla systemu Windows. Wyświetla aktualną prędkość Internetu oraz użycie procesora i pamięci RAM. Istnieją również inne możliwości, takie jak wbudowany wyświetlacz w pasku zadań, zmienne skórki wyświetlacza i historyczne statystyki ruchu. 

# Powiązane linki

Aby pobrać najnowszą wersję TrafficMonitor, [kliknij tutaj](https://github.com/zhongyang219/TrafficMonitor/releases/latest).

Alternatywny link: Pobierz z [Dysk sieciowy Baidu](https://pan.baidu.com/s/15PMt7s-ASpyDwtS__4cUhg). Kod dostępu: `ou0m`  

Stronę projektu na Gitee znajdziesz pod adresem [clicking here](https://gitee.com/zhongyang219/TrafficMonitor)

Jeśli napotkasz jakiekolwiek problemy, [kliknij tutaj](./Help_pl-pl.md), aby zapoznać się z odpowiedziami na najczęściej zadawane pytania.  

Możesz również [kliknąć tutaj](https://github.com/zhongyang219/TrafficMonitor/actions?query=workflow:"Release+CI"), aby pobrać wersję przedpremierową narzędzia TrafficMonitor.

W wersji 1.80 i nowszych dodano funkcję monitorowania temperatury. Jeśli użytkownik nie potrzebuje funkcji monitorowania temperatury i napotyka problemy z wersją 1.80 lub nowszą, zaleca się pobranie wcześniejszej wersji bez monitora temperatury (wersja Lite). (Wersję `Lite` można znaleźć na stronie Release.)

TrafficMonitor opiera się na środowisku operacyjnym Microsoft Visual C++. Jeśli podczas uruchamiania programu pojawi się komunikat o błędzie "Cannot find MSVC*.dll", kliknij poniższy link, aby pobrać i zainstalować środowisko operacyjne Microsoft Visual C++.

[Pobierz najnowszą obsługiwaną wersję pakietu redystrybucyjnego Visual C++ | Dokumentacja firmy Microsoft](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)

# Opis wersji

Istnieją dwie wersje TrafficMonitor: wersja standardowa i wersja Lite. Wersja standardowa zawiera wszystkie funkcje, podczas gdy wersja Lite nie zawiera funkcji monitorowania sprzętu, takich jak monitorowanie temperatury, użycie GPU i użycie dysku twardego. Wersja standardowa wymaga uprawnień administratora, podczas gdy wersja Lite ich nie wymaga. 

Jeśli nie ma potrzeby monitorowania temperatury i innych informacji o sprzęcie, zaleca się korzystanie z wersji Lite.

Poniżej znajduje się porównanie funkcji obu wersji.

| Funkcje                                       | Wersja standard  | Wersja Lite  |
| --------------------------------------------- | ---------------- | ------------ |
| Monitorowanie prędkości Internetu             | ✔                | ✔            |
| Monitorowanie wykorzystania CPU i pamięci RAM | ✔                | ✔            |
| CPU, GPU, dysk twardy, temper. płyty głównej  | ✔                | ❌            |
| Monitorowanie częstotliwości CPU              | ✔                | ✔            |
| Monitorowanie wykorzystania GPU               | ✔                | ❌            |
| Monitorowanie wykorzystania Dysku Twardego    | ✔                | ❌            |
| Szczegóły połączenia sieciowego               | ✔                | ✔            |
| System z wtyczką                              | ✔                | ✔            |
| Zmiana skórki głównego okna                   | ✔                | ✔            |
| Wymagane uprawnienia administratora           | Tak              | Nie          |

# Główne cechy

* Wyświetla aktualną prędkość transmisji sieciowej oraz użycie procesora i pamięci RAM
* Jeśli dostępnych jest wiele kart sieciowych, użytkownicy mogą automatycznie lub ręcznie wybierać sieci, z którymi chcą się połączyć
* Wyświetla szczegóły połączenia sieciowego
* Obsługuje wbudowany wyświetlacz na pasku zadań
* Obsługuje zmianę skórki i konfigurowalne skórki
* Historyczne statystyki ruchu
* Monitorowanie informacji o sprzęcie
* System wtyczek

# Instrukcja użytkowania

**[Kliknij tutaj](https://github.com/zhongyang219/TrafficMonitor/wiki) aby przejść na stronę Wiki i zapoznać się ze szczegółową dokumentacją TrafficMonitor (dostępną tylko w języku chińskim).**

# Zrzuty ekranu

Główne okno pływające:  
![](./Screenshots/en_us/main1.png)  
Menu prawego przycisku myszy:  
![](./Screenshots/en_us/main.png)  
Okno paska zadań  

![](./Screenshots/en_us/taskbar.png)  

Kolorowe skórki:  

![](./Screenshots/skins.PNG)  

Zmień skórki:  

![](./Screenshots/en_us/selecte_skin.png)  

Opcje:  

<img src="./Screenshots/en_us/option.jpg" style="zoom:80%;" />  

# Jak używać

Podczas uruchamiania programu na ekranie pojawi się pływające okno pokazujące prędkość sieci. Kliknij prawym przyciskiem myszy pływające okno, aby otworzyć menu podręczne.

TrafficMonitor może wyświetlać informacje na pasku zadań. Jednak domyślne ustawienie TrafficMonitor pokazuje tylko okno główne (okno pływające), aby wyświetlić osadzony wyświetlacz na pasku zadań, kliknij prawym przyciskiem myszy okno i wybierz „Show Taskbar Window” w menu podręcznym.

Użytkownicy mogą dostosowywać wyświetlane elementy w osadzonym oknie paska zadań. Domyślnie wyświetlana jest tylko prędkość sieci. Aby wyświetlić użycie procesora i pamięci RAM lub inne informacje, wybierz „Ustawienia wyświetlania” w menu prawym przyciskiem myszy okna paska zadań i zaznacz elementy, które chcesz wyświetlić w oknie dialogowym „Ustawienia wyświetlania”, jak pokazano poniżej:

<img src="./Screenshots/en_us/taskbar_item_settings.png" style="zoom:80%;" />

# Dostosowywalna skórka
<img src="./Screenshots/selecte_skin.png" style="zoom:80%;" />

Możesz wybrać “Inne funkcje“-“Zmień skórkę“ w menu prawego przycisku myszy głównego okna lub ikony obszaru powiadomień, aby otworzyć interfejs zmiany skórki. Użytkownicy mogą również pobrać więcej skórek i dostosować je [tutaj](https://github.com/zhongyang219/TrafficMonitorSkin/blob/master/皮肤下载.md) zgodnie z własnymi potrzebami.

Pliki skórek są przechowywane w katalogu `skins` w katalogu, w którym znajduje się aplikacja. Każda skórka jest przechowywana w swoim indywidualnym folderze, a nazwa folderu jest nazwą skórki. 

Wśród plików  `background.bmp` i `background_l.bmp` znajdują się obrazy tła, `skin.ini` jest dokumentem konfiguracji skórki. Użytkownicy mogą w nim dostosować kolory tekstu, czcionki, kreatora skórki, a także rozmiar i położenie każdego elementu itd. 

Od wersji 1.80 dodano format xml pliku konfiguracji skórki `skin.xml`. Bez dokumentu konfiguracji skórki xml TrafficMonitor nie będzie w stanie wyświetlić temperatury i użycia GUP.

Od wersji 1.85 dodano obsługę obrazów tła w formacie png, możesz użyć formatu png do tworzenia skórek z przezroczystym tłem. Nazwy plików obrazów tła to `background.png` i `background_l.png`.

Kliknij poniższy link, aby uzyskać szczegółową instrukcję dotyczącą tworzenia skór: 

[皮肤制作教程 · zhongyang219/TrafficMonitor Wiki (github.com)](https://github.com/zhongyang219/TrafficMonitor/wiki/皮肤制作教程)

# Konfigurowanie opcji
<img src="./Screenshots/option.jpg" style="zoom:80%;" /><br>
Wybranie "Opcje..." w menu podręcznym umożliwia użytkownikom dostęp do konfiguracji opcji. W oknie dialogowym konfiguracji opcji użytkownik może ustawić kolor tekstu, czcionkę tekstu, kolor tła, jednostkę prędkości sieci i wyświetlany tekst itp. oddzielnie dla okna głównego i okna zadania.

W "Regular Configurations" użytkownicy mogą zmienić ustawienia dotyczące tego, czy program przeprowadza automatyczną aktualizację i czy program uruchamia się automatycznie po uruchomieniu komputera. Użytkownicy mogą również skonfigurować, kiedy powiadomienia mają być wysyłane. 

W wersji 1.72 i nowszych użytkownicy mogą konfigurować kolory tekstu dla każdego elementu indywidualnie. Zaznacz opcję "Wyznacz kolory dla każdego elementu", kliknij pole koloru po prawej stronie "kolorów tekstu", aby wyświetlić okno dialogowe ze szczegółową konfiguracją kolorów, w którym użytkownicy mogą wyznaczać kolory dla każdego elementu indywidualnie. 

# System z wtyczką

Dla wersji 1.82 i nowszych dodano system wtyczek. Wtyczka dll musi być umieszczona w katalogu "plugins", na tym samym poziomie katalogu co "TrafficMonitor.exe". Wtyczka powinna ładować się automatycznie podczas uruchamiania programu. Użytkownicy mogą przeglądać i zarządzać załadowanymi wtyczkami w menu podręcznym "Więcej funkcji"-"Zarządzanie wtyczkami".

Instrukcje dotyczące tworzenia wtyczek dla TrafficMonitor można znaleźć w [Plugin Development Guide · zhongyang219/TrafficMonitor Wiki (github.com)](https://github.com/zhongyang219/TrafficMonitor/wiki/Plugin-Development-Guide).

To download the TrafficMonitor plugins, please [click here](https://github.com/zhongyang219/TrafficMonitorPlugins/blob/main/download/plugin_download.md).

# O funkcji monitorowania sprzętu

W przypadku wersji 1.80 i nowszych funkcje monitorowania sprzętu (w tym monitorowanie temperatury, częstotliwości procesora i wykorzystania GPU) zostały dodane do TrafficMonitor. Opiera się ona na bibliotece open source [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor). Jeśli napotkasz problemy z korzystaniem z funkcji monitorowania temperatury, [kliknij tutaj](./Help_en-us.md#13-about-the-temperature-monitoring-of-trafficmonitor). 

Należy również zauważyć, że funkcja monitorowania temperatury jest domyślnie wyłączona. Jeśli użytkownicy chcą korzystać z funkcji monitorowania temperatury w TrafficMonitor, przejdź do ["Ustawienia opcji"-"Ustawienia ogólne"-"Monitorowanie sprzętu"](https://github.com/zhongyang219/TrafficMonitor/wiki/选项设置#硬件监控), aby ją włączyć.

**Uwaga: Funkcja monitorowania sprzętu (w tym monitorowanie temperatury i monitorowanie wykorzystania GPU) może nadal mieć pewne problemy, które mogą zużywać więcej procesora i pamięci RAM. Według opinii niektórych użytkowników włączenie funkcji temperatury powoduje problemy, takie jak awarie programu i systemu itp. Należy rozważyć powyższe zagrożenia przed włączeniem funkcji monitorowania sprzętu. W przeciwnym razie nie należy korzystać z funkcji monitorowania sprzętu.**

# Aktualizuj dziennik

**[Kliknij tutaj, aby wyświetlić dziennik aktualizacji.](./UpdateLog/update_log_pl-pl.md)**

