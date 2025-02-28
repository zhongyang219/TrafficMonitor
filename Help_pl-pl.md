**[简体中文](./Help.md) | Polish**

#TrafficMonitor Często zadawane pytania
To strona o Często zadawanych pytaniach TrafficMonitor. Jeśli chcesz zobaczyć szczegółowe wprowadzenie do funkcji i użytkowania TrafficMonitor, [kliknij tutaj](https://github.com/zhongyang219/TrafficMonitor/wiki), aby przejść na stronę TraffinMonitor Wiki (dostępna tylko w języku chińskim).

### "Okno paska zadań pojawia się po lewej stronie paska zadań" nie działa w systemie Windows 11 w wersji 1.84

W wersji 1.84, jeśli okno paska zadań jest wyrównane do lewej, okno paska zadań zawsze będzie wyświetlane po prawej stronie, ponieważ nakłada się na oryginalny przycisk paska zadań, gdy jest wyświetlane po lewej stronie.
Program wykrywa wyrównanie paska zadań, odczytując wartość klucza "TaskbarAl" w rejestrze "\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced". Ale jeśli nigdy nie zmienisz ustawień wyrównania paska zadań, ten klucz nie istnieje i TrafficMonitor błędnie myśli, że pasek zadań jest wyrównany do lewej. To błąd. Rozwiązanie jest następujące:
Kliknij prawym przyciskiem myszy na pasku zadań, wybierz Ustawienia paska zadań i zmień metodę wyrównania paska zadań raz. Po wygenerowaniu powyższej wartości klucza, na przykład, zmień "centrum" na "Wyrównaj do lewej", a następnie zmień go z powrotem na "centrum".

### Jak wyświetlić wykorzystanie procesora i pamięci?

Kliknij prawym przyciskiem myszy okno główne i zaznacz "Pokaż więcej informacji". Jeśli chcesz również wyświetlić użycie procesora i pamięci w oknie paska zadań, kliknij prawym przyciskiem myszy okno paska zadań i zaznacz "Użycie procesora i pamięci" w podmenu "Ustawienia wyświetlania<img src="./Screenshots/images/item.png" style="zoom: 80%;" />".

### Jak zmienić tekst taki jak „CPU” i „MEM” na pasku zadań

Tekst etykiety w oknie paska zadań można dostosować. Kliknij prawym przyciskiem myszy w oknie paska zadań, wybierz "Opcje", kliknij "Ustawienia systemu Windows paska zadań", kliknij przycisk "Ustawienia wyświetlania tekstu", tekst po prawej stronie elementu można zmodyfikować, klikając dwukrotnie. Po zmodyfikowaniu kliknij przycisk "OK" dwa razy.

<img src="Screenshots/images/image-20221119093547649.png" alt="image-20221119093547649" style="zoom:80%;" />

Ponieważ tekst jest konfigurowalny, nie zmienia się automatycznie po zmianie języka. Po wybraniu języka możesz kliknąć przycisk "Przywróć domyślne" w oknie dialogowym "Ustawienia wyświetlania tekstu".

### Prędkość sieci jest zawsze wyświetlana jako 0 KB

Może to być spowodowane zmianą sieci, z której korzysta Twój komputer.

Problem można rozwiązać, klikając opcję "Odśwież listę połączeń" w menu kontekstowym pod pozycją "Wybierz połączenia sieciowe".

<img src="Screenshots/images/image-20221119094324759.png" alt="image-20221119094324759" style="zoom:80%;" />

Jeżeli problem nadal występuje, spróbuj wybrać połączenie sieciowe, które chcesz monitorować, w podmenu "Wybierz połączenia sieciowe" zamiast wybierać opcję "Wybór automatyczny".

Jeśli problem nadal występuje, spróbuj kliknąć przycisk "Wybierz połączenie do monitorowania" w "Ustawieniach opcji" > "Ustawienia ogólne" > "Zaawansowane", zaznacz sieć, którą chcesz monitorować, odznacz inne pozycje i kliknij przycisk "OK" dwa razy.

<img src="Screenshots/images/image-20221119094501312.png" alt="image-20221119094501312" style="zoom:80%;" />

### Jak ustawić kolor każdego elementu w oknie paska zadań osobno?

Wybierz "Opcje". W menu prawym przyciskiem myszy przejdź do karty "Ustawienia okna paska zadań", zaznacz "Określ kolory dla każdego elementu", a następnie kliknij blok kolorów po prawej stronie "Koloru tekstu", aby wyświetlić okno dialogowe ustawień kolorów okna paska zadań. 

Jeśli nie zaznaczysz opcji "Określ kolory dla każdego elementu", możesz ustawić tylko jednolity kolor tekstu.

### "Automatyczne uruchamianie podczas startu systemu Windows" nie działa
Począwszy od wersji 1.80, wersja z monitorowaniem temperatury i wersja bez monitorowania temperatury przyjęły różne metody realizacji "automatycznego uruchamiania podczas startu systemu Windows".

* Wersje bez monitorowania temperatury i wersje przed wersją 1.80:

  Funkcja automatycznego uruchamiania wersji bez monitorowania temperatury i wersji przed wersją 1.80 jest realizowana poprzez utworzenie klucza "TrafficMonitor" w ścieżce rejestru "Computer\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run".
Jeśli napotkasz problem z automatycznym uruchamianiem, sprawdź, czy rejestr istnieje, a następnie sprawdź, czy ścieżka programu jest poprawna. Jeśli zmienisz lokalizację programu, automatyczne uruchamianie będzie nieprawidłowe z powodu nieprawidłowej lokalizacji programu. W takim przypadku wystarczy odznaczyć opcję "Automatyczne uruchamianie podczas uruchamiania systemu Windows" w ustawieniach opcji, a następnie ją zaznaczyć.<br>

  Należy pamiętać, że niektóre zewnętrzne oprogramowanie zabezpieczające może uniemożliwić automatyczne uruchomienie TrafficMonitor. Spróbuj zezwolić TrafficMonitor na automatyczne uruchomienie w oprogramowaniu zabezpieczającym.

  Jeśli ustawisz program do uruchamiania jako administrator, funkcja automatycznego uruchamiania również nie będzie działać. Spróbuj usunąć uruchamianie jako administrator.<br>

* Wersja z monitorowaniem temperatury:

  Wersja obejmująca monitorowanie temperatury implementuje funkcję automatycznego uruchamiania poprzez utworzenie planu zadań. Harmonogram zadań można otworzyć za pomocą `Panelu sterowania\Systemu i zabezpieczeń\Narzędzi administracyjnych`. Jak pokazano poniżej:

  <img src="./Screenshots/images/image3.jpg"/>

  Jeśli napotkasz sytuację, w której program nie może zostać uruchomiony automatycznie po uruchomieniu, przejdź do "Harmonogramu zadań", aby sprawdzić, czy zaplanowane zadanie TrafficMonitor jest tworzone normalnie i czy ścieżka do pliku exe jest prawidłowa.

Częstym powodem, dla którego "Automatyczne uruchamianie podczas uruchamiania systemu Windows" nie działa, jest to, że mogłeś zmienić lokalizację głównego programu TrafficMonitor. Jeśli skonfigurowałeś TrafficMonitor tak, aby uruchamiał się automatycznie podczas uruchamiania, ale przeniesiesz TrafficMonitor do innej lokalizacji, automatyczne uruchamianie podczas uruchamiania z pewnością się nie powiedzie. Musisz otworzyć "Ustawienia opcji"-"Ustawienia ogólne" TrafficMonitor, jeśli zaznaczono "Automatyczne uruchamianie podczas uruchamiania systemu Windows", usuń zaznaczenie, a następnie ponownie otwórz "Ustawienia opcji"-"Ustawienia ogólne" i ponownie zaznacz "Automatyczne uruchamianie podczas uruchamiania systemu Windows" będzie w porządku.

Należy zauważyć, że jeśli użyjesz wersji bez monitorowania temperatury, aby utworzyć element automatycznego uruchamiania w rejestrze, a następnie użyjesz wersji z monitorowaniem temperatury, aby włączyć funkcję "Automatyczne uruchamianie podczas uruchamiania systemu Windows", automatycznie usunie ona element automatycznego uruchamiania w rejestrze, a następnie utworzy element automatycznego uruchamiania w planie zadań. odwrotnie.

### Program wyświetla okno komunikatu "Nie można zapisać ustawień".
Jeśli napotkałeś ten problem, oznacza to, że program nie ma uprawnień do zapisu danych w swoim katalogu, co powoduje, że danych ustawień nie można zapisać. Spróbuj przenieść program do innego folderu z uprawnieniami do zapisu, aby rozwiązać ten problem. <br>
Możesz również zmienić ścieżkę zapisu pliku konfiguracji i danych na katalog C:\Users\\<username\>\AppData\Roaming\TrafficMonitor, wykonując następujące kroki. <br>
* Wyjdź z TrafficMonitor i uruchom ponownie TrafficMonitor jako administrator.
* Wybierz "Opcje" w menu prawym przyciskiem myszy, przejdź do zakładki "Ustawienia ogólne" i wybierz "Zapisz w katalogu AppData" w "Pliki konfiguracji i danych".

Jeśli nadal wyświetla się okno komunikatu "Nie można zapisać ustawień", otwórz katalog, w którym znajduje się aplikacja, otwórz plik `global_cfg.ini`. Jeśli nie istnieje, utwórz nowy, dodając następujące informacje:

```
[config]
portable_mode = true
```

Jeśli nie można utworzyć pliku, możesz utworzyć plik w innej lokalizacji, takiej jak pulpit, a następnie przenieść go do katalogu, w którym znajduje się program.

Jeśli plik `global_cfg.ini` już istnieje, zmień wartość `portable_mode` na `true` i zapisz, a następnie uruchom ponownie TrafficMonitor.

Jeśli `global_cfg.ini` nie ma uprawnień do zapisu, możesz spróbować skopiować plik na pulpit, a następnie skopiować go z powrotem do oryginalnej ścieżki, aby nadpisać oryginalny plik po modyfikacji.

Po wykonaniu tych kroków ten problem teoretycznie nie powinien wystąpić. Jeśli ten problem nadal występuje, spróbuj usunąć plik C:\Users\\<username\>\AppData\Roaming\TrafficMonitor\config.ini. Ten plik zostanie ponownie wygenerowany po usunięciu.

W wersji 1.79 i nowszych, jeśli katalog, w którym znajduje się program, nie jest zapisywalny, pliki konfiguracji i danych zostaną automatycznie zapisane w katalogu "C:\Users\\<username\>\AppData\Roaming\TrafficMonitor". W tym momencie "Opcje" - "Ustawienia ogólne" - "Zapisz do katalogu programu" w „Pliki konfiguracji i danych” nie będą dostępne.

### Konfiguracja zostanie utracona po następnym uruchomieniu

Jeśli nie widzisz okna komunikatu "Nie można zapisać ustawień", oznacza to, że plik konfiguracyjny można zapisać normalnie. Przyczyną tego problemu może być to, że na komputerze znajduje się wiele plików aplikacji TrafficMonitor, a pliki konfiguracyjne są zapisywane w katalogu aplikacji.

Na przykład otworzyłeś "D:\software\TrafficMonitor\TrafficMonitor.exe" i zmodyfikowałeś konfigurację, ale następnym razem, gdy uruchomisz komputer, zostanie uruchomiony "D:\software1\TrafficMonitor\TrafficMonitor.exe", podczas gdy zmodyfikowane wcześniej pliki konfiguracyjne zostaną zapisane w "D:\software\TrafficMonitor\\".

Metoda rozwiązania tego problemu jest następująca:

1. Zmień lokalizację zapisu plików konfiguracyjnych na katalog AppData w "Ustawieniach opcji" > "Ustawienia ogólne" > "Plik konfiguracji i danych".
2. Usuń niepotrzebne pliki aplikacji TrafficMonitor na swoim komputerze, uruchom TrafficMonitor, otwórz "Ustawienia opcji" > "Ustawienia ogólne", kliknij przycisk "Resetuj autorun".

### Okno zawieszenia może być wyświetlane tylko na głównym monitorze, gdy obecnych jest wiele monitorów.
Domyślnie okna zawieszenia nie można przenieść poza granice ekranu. Jeśli musisz przenieść okno zawieszenia na inne monitory, otwórz "Opcje" > "Ustawienia głównego okna", klikając prawym przyciskiem myszy okno zawieszenia, zaznacz "Zezwalaj poza granice ekranu" i kliknij przycisk "OK". W tym momencie okno zawieszenia nie jest już ograniczone do ekranu, można je również przenieść na dowolne monitory.

Jeśli usuniesz dodatkowy monitor, okno zawieszenia może pojawić się poza obszarem ekranu, powodując niewidoczność. W tym momencie wystarczy kliknąć prawym przyciskiem myszy ikonę w obszarze powiadomień, wybrać "Opcje", odznaczyć „Zezwalaj poza granice ekranu” w "Ustawieniach głównego okna", a okno zawieszenia ponownie pojawi się w obszarze ekranu.

### Wartość prędkości sieci nie jest w pełni wyświetlana.
Ponieważ szerokość każdego znaku w różnych czcionkach nie jest taka sama, w niektórych przypadkach wydaje się, że problem wartości prędkości sieci nie jest w pełni wyświetlany. Jeśli wystąpi ten problem, otwórz "Opcje" - "Ustawienia okna paska zadań" i wybierz większą wartość z listy rozwijanej "Liczba cyfr".
### Jak anulować po ustawieniu penetracji myszy?
Kliknij prawym przyciskiem myszy ikonę TrafficMonitor w obszarze powiadomień, odznacz "Penetracja myszy".<br>
Po ustawieniu penetracji myszy okno zawieszenia nie będzie mogło reagować na żadne komunikaty myszy ani menu podręczne prawego przycisku myszy. Ale menu można wyświetlić, klikając prawym przyciskiem myszy ikonę w obszarze powiadomień. Menu prawego przycisku myszy głównego okna i która ikona obszaru powiadomień jest dokładnie taka sama.<br>
Ponadto, nawet jeśli wcześniej ustawiłeś ukrytą ikonę powiadomień, po ustawieniu "Penetracji myszy" ikona powiadomień pojawi się automatycznie, aby zapobiec wyświetlaniu menu prawego przycisku myszy. <br>
Uwaga: ikona obszaru powiadomień będzie automatycznie wyświetlana w następujących sytuacjach: <br>
* Po ustawieniu "Mouse Penetrate";
* Ukryj okno główne bez wyświetlania okna paska zadań;
* Zamknij okno paska zadań, gdy okno główne jest ukryte;
* Zamknij okno paska zadań, gdy ustawiono "Mouse Penetrate".
### Problemy z kolorem okna paska zadań w białym motywie paska zadań w systemie Windows 10
Podczas korzystania z białego motywu paska zadań możesz kliknąć przycisk "Ustawienie wstępne" w "Ustawienia okna paska zadań" i wybrać "Tryb światła", aby ustawić kolor paska zadań w trybie jasnym. Jak pokazano na rysunku:

<img src="./Screenshots/images/image2.jpg" style="zoom:80%;" />

Jednocześnie możesz również zaznaczyć opcję "Automatyczna adaptacja do ciemnego/jasnego motywu systemu Windows 10", program automatycznie zmieni ustawienie kolorów, gdy zmieni się ciemny/jasny motyw systemu Windows 10. Możesz kliknąć przycisk "Ustawienia automatycznej adaptacji", aby skonfigurować, które ustawienie kolorów ma być używane dla ciemnych i jasnych motywów.

### Okien paska zadań nie można wyświetlić, gdy w systemie Windows 10 włączony jest tryb HDR

Niektórzy użytkownicy zgłaszali, że włączenie funkcji HDR w systemie Windows 10 powoduje, że okno paska zadań nie wyświetla się. Jeśli napotkasz ten problem, możesz spróbować wyłączyć opcję "Tło przezroczyste" w ["Ustawienia opcji" - "Ustawienia okna paska zadań"](https://github.com/zhongyang219/TrafficMonitor/wiki/选项设置#任务栏窗口设置).

### O monitorowaniu temperatury TrafficMonitor

Ze względu na pewne problemy występujące w niektórych komputerach spowodowane funkcją monitorowania temperatury, funkcja monitorowania temperatury jest domyślnie wyłączona. Jeśli chcesz użyć funkcji monitorowania temperatury TrafficMonitor, przejdź do ["Ustawienia opcji"-"Ustawienia ogólne"-"Monitorowanie sprzętu"](https://github.com/zhongyang219/TrafficMonitor/wiki/选项设置#硬件监控), aby ją włączyć. Po jej włączeniu elementy związane z temperaturą pojawią się w podmenu "Ustawienia wyświetlania" w menu prawym przyciskiem myszy na pasku zadań.

Funkcja monitorowania temperatury TrafficMonitor opiera się na bibliotece open source innej firmy [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor). Jeśli napotkasz problem, że temperatura sprzętu nie może zostać wyświetlona lub wyświetlana temperatura jest nieprawidłowa, pobierz najpierw LibreHardwareMonitor i sprawdź, czy LibreHardwareMonitor może normalnie wyświetlić odpowiednią temperaturę.

Jeśli LibreHardwareMonitor nie może wyświetlić temperatury odpowiedniego sprzętu, nie mogę rozwiązać tego problemu. Możesz zgłosić swój problem autorowi LibreHardwareMonitor.

Jeśli LibreHardwareMonitor może normalnie wyświetlić temperaturę odpowiedniego sprzętu, zgłoś ten problem mnie i prześlij zrzut ekranu LibreHardwareMonitor, abym mógł zbadać Twój problem. 

**Uwaga: Funkcja monitorowania sprzętu (w tym monitorowanie temperatury i monitorowanie wykorzystania GPU) może mieć pewne problemy, które mogą powodować większe wykorzystanie procesora i pamięci. Według opinii niektórych użytkowników włączenie funkcji temperatury spowoduje problemy, takie jak awarie programów i awarie systemu. Podejmij decyzję o włączeniu funkcji monitorowania sprzętu po zapoznaniu się z powyższymi zagrożeniami. W przeciwnym razie nie korzystaj z funkcji monitorowania sprzętu.**

## Podczas uruchamiania programu pojawia się komunikat, że nie można znaleźć pliku "MSVC\*.dll" lub "mfc\*.dll"s

Kliknij poniższy link, aby pobrać i zainstalować środowisko uruchomieniowe Microsoft Visual C++.

[Najnowsze obsługiwane pliki do pobrania Visual C++ Redistributable | Dokumentacja firmy Microsoft](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)

<br><br>

>Jeśli napotkasz problem awarii programu, wyłącz wszystkie elementy w "Opcje" - "Ustawienia ogólne" - "Monitorowanie sprzętu" po ponownym uruchomieniu programu, ponieważ na podstawie pliku DMP dostarczonego przez użytkownika, większość problemów z awariami jest związana z funkcją monitorowania sprzętu. Nie mogę rozwiązać problemu awarii związanego z funkcją monitorowania sprzętu, proszę nie wysyłaj mi opinii e-mailem. Jeśli wykluczyłeś problem z monitorowaniem sprzętu, ale problem awarii nadal występuje, przejdź do "Opcje" - "Ustawienia ogólne" - "Zaawansowane" - "Zarządzanie wtyczkami", aby wyłączyć wszystkie wtyczki, a następnie uruchom ponownie program. Jeśli wykluczyłeś problem z wtyczką, ale problem awarii nadal występuje, proszę wyślij mi e-mail zgodnie z monitami w oknie awarii. >
>Jeśli napotkałeś jakiekolwiek inne problemy, możesz również kliknąć "Skontaktuj się z autorem" w oknie dialogowym "Informacje" lub bezpośrednio [kliknij tutaj](mailto:zhongyang219@hotmail.com), aby wysłać mi wiadomość e-mail.<br>
>Opisz szczegółowo napotkane problemy, monity o błędach, jakie operacje próbowałeś, najlepiej załącz zrzuty ekranu i pliki konfiguracyjne ("Opcje"-"Ustawienia ogólne"-"Otwórz ścieżkę pliku konfiguracyjnego"). <br>
>Uwaga: Przed wysłaniem wiadomości e-mail potwierdź adres e-mail, którego użyłeś. Jeśli Twój adres e-mail ma postać "Outlook_\<losowy ciąg\>@outlook.com", taki adres e-mail nie może odpowiedzieć.
>Może to być spowodowane faktem, że używasz adresu e-mail innej firmy jako konta Microsoft do logowania się do systemu Windows. Jeśli tak jest, upewnij się, że w wiadomości podałeś prawidłowy adres e-mail.
