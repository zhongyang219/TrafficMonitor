**[简体中文](./update_log.md) | [繁體中文](./update_log_pl-pl.md) | Polish**

# Dziennik aktualizacji TrafficMonitor
## V1.85.1 (2025/02/10)

* Naprawiono problem, polegający na tym, że podczas korzystania z oprogramowania innych firm w celu zmiany na klasyczny pasek zadań w systemie Windows 11 opcja "Okno paska zadań pojawia się po lewej stronie paska zadań" w Ustawieniach okna paska zadań jest niedostępna.

* Naprawiono problem, polegający na tym, że położenie okien paska zadań jest nieprawidłowe, gdy na pasku zadań nie ma żadnej ikony. #1913
* Naprawiono problem, polegający na tym, że "Szerokość danych prędkości sieci" nie działa po kliknięciu "Zastosuj" w Ustawieniach okna paska zadań. #1916
* Ustawienia związane z systemem Windows 11 w Ustawieniach okna paska zadań przeniesiono do nowego okna dialogowego. Dodano opcję, aby uniknąć nakładania się z prawymi widżetami.
* Naprawiono problem, polegający na tym, że gdy "CPU" w Monitorowaniu sprzętu nie jest włączone, "Metoda pozyskiwania użycia procesora" zostanie zmieniona z powrotem na "Na podstawie czasu procesora" po ustawieniu jej na "Użyj licznika wydajności".
* Naprawiono problem, chaotycznych znaków w oknie dialogowym potwierdzenia.
* Naprawiono problem, w którym wyświetlana prędkość sieci mogła być większa niż rzeczywista prędkość sieci, gdy "Interwały monitorowania" były ustawione zbyt krótko. #1263 #1674
* Naprawiono problem, w którym "Dostępna pamięć" w oknie paska zadań nie mogła być w pełni wyświetlana, gdy pamięć przekraczała 100G. #1722

## V1.85 (2025/01/21)

**Aktualizacje funkcji:**

* Dodano metodę renderowania Direct2D do okna paska zadań, co rozwiązuje problem, że czasami krawędzie czcionek są postrzępione. #1156
* Dodano obsługę kart graficznych Intel dla funkcji monitorowania karty graficznej.
* Dodano menu wtyczki do menu kontekstowego po kliknięciu prawym przyciskiem myszy elementu wtyczki.
* Dodano ikonę wtyczki w oknie dialogowym zarządzania wtyczkami, dodaj polecenie wtyczki dla menu kontekstowego.
* Maksymalna wartość interwału monitorowania jest dostosowana do 30 sekund.
* Dodano opcje "Przesunięcie okna u góry" i "Margines pionowy" w Ustawieniach okna paska zadań.
* Dodano obsługę przezroczystych obrazów tła w formacie png do okna głównego.
* Dodano funkcję automatycznego przełączania skórek zgodnie z ciemnym/jasnym trybem kolorów systemu Windows. #1013
* Dodano funkcję wyświetlania wykresu wykorzystania zasobów elementów wtyczki na pasku zadań i zaktualizowano odpowiedni interfejs wtyczki. #1077
* Dodano wyświetlanie dzisiejszego całkowitego ruchu w oknie głównym i oknie paska zadań. #1174
* Dodano opcję "Włącz kolorowe emoji" w ustawieniach okna paska zadań (tylko renderowanie Direct2D).
* Górny limit powiadomień o temperaturze w ustawieniach ogólnych dostosowany do 120 stopni. #1814
* Dodano obsługę Arm64EC. #1703
* Dodano możliwość wyświetlania częstotliwości procesora w wersji Lite.
* Dodano domyślny kolor tekstu do "Ustawienie wstępne 2" i "Ustawienie wstępne 3" w Ustawieniach paska zadań - Ustawienie wstępnePreset.

**Problem rozwiązany:**
* LibreHardwareMonitorLib.dll został zaktualizowany do wersji 0.9.4. Rozwiązano problem, przez który informacje o monitorze sprzętowym niektórych urządzeń nie mogły być wyświetlane poprawnie.
* Naprawiono problem, przez który położenie okna paska zadań było nieprawidłowe dla urządzeń z ekranem dotykowym po kompilacji 22621 systemu Windows 11.
* Naprawiono problem, przez który położenie okna paska zadań było nieprawidłowe, gdy widżety nie istniały w systemie Windows 11. #1582
* Naprawiono problem, przez który okno paska zadań nakładało się na widżety systemowe, gdy było wyświetlane po prawej stronie w systemie Windows 11.
* Naprawiono problem, zbyt szybkiego przewijania podczas korzystania z gestów touchpada w oknie dialogowym Ustawienia opcji.
* Naprawiono problem, przez który wyświetlanie częstotliwości procesora na czubku myszy w oknie głównym było nieprawidłowe.
* Naprawiono problem, przez który tekst wyświetlany dla elementów wtyczki w plikach skórki był nieprawidłowy. #1813
* Naprawiono problem, przez który jeśli ikona obszaru powiadomień była ukryta, pojawiała się ponownie po wyświetleniu okna podręcznego powiadomienia. #1814 #1812 #1811

## V1.84.1 (2022/11/18)

* Naprawiono problem, przez który okno paska zadań nie mogło być wyświetlane po lewej stronie w systemie Windows 11.
* Naprawiono problem, przez który informacje w oknie paska zadań nie mogły być w pełni wyświetlane, gdy pasek zadań znajdował się po lewej lub prawej stronie pulpitu.
* Naprawiono problem, w systemie Windows 11 22H2, gdy wszystkie okna były zamknięte, a na pasku zadań nie było żadnych ikon, położenie okna paska zadań było nieprawidłowe.
* Dodano obsługę długiego naciśnięcia w celu wyświetlenia menu kontekstowego na urządzeniach z ekranem dotykowym.
* Naprawiono problem, przez który położenie okna paska zadań było nieprawidłowe z powodu szerokiego przycisku wyszukiwania w najnowszej wersji systemu Windows 11.
* Naprawiono problem, przez który rozmiar okna dialogowego "Informacje" był nieprawidłowy.

## V1.84 (2022/10/05)

* Naprawiono problem, polegający na tym, że położenie okna paska zadań nie było prawidłowe w systemie Windows 11 22H2.
* Naprawiono problem, polegający na tym, że okno paska zadań nakładało się na widżety systemowe, gdy okno paska zadań było wyświetlane po lewej stronie w systemie Windows 11.
* Dodano wyświetlanie częstotliwości procesora.
* Naprawiono problem, polegający na tym, że w oknie głównym brakowało "/s" prędkości sieci.
* Naprawiono problem,, który mógł powodować niewystarczający bufor ciągu podczas zapisywania plików INI.
* Zaktualizowano interfejs wtyczki, który umożliwia zdarzenia klawiatury odpowiedzi wtyczki.
* Naprawiono problem, polegający na tym, że okno paska zadań nie wyświetlało się prawidłowo po przełączeniu się między różnymi monitorami DPI.
* Naprawiono problem, w którym okno dialogowe "Opcje" pojawiało się po wybraniu "Szczegóły" z menu kontekstowego w oknie dialogowym "Zarządzanie wtyczkami".
* Zaktualizowano bibliotekę HardwareMonitor, aby rozwiązać problem polegający na tym, że nie można uzyskać temperatury procesora Intel 12. generacji.

## V1.83 (2022/02/03)

* Naprawiono problem, polegający na tym, że gdy istnieje już plan zadań automatycznego uruchamiania, automatyczne uruchamianie przy uruchomieniu nie zostanie ustawione.
* Dodano okno dialogowe "Program jest już uruchomiony" podczas uruchamiania programu, gdy jest już uruchomiony proces TrafficMonitor.
* Dodano informacje o wtyczce w podpowiedziach narzędzi myszy.
* Naprawiono problem, w którym wtyczki mogły być nadal ładowane, jeśli były wyłączone.
* Dodano obsługę swobodnego sortowania elementów wtyczek na pasku zadań.
* Dodano funkcję wybierania połączeń sieciowych, które mają być wyświetlane na "Liście połączeń sieciowych" w ustawieniach opcji.
* Dodano funkcję wyświetlania całkowitej szybkości sieci.
* Dodano funkcję wyświetlania wykresu szybkości sieci na pasku zadań.
* Naprawiono problem, polegający na tym, że można wyświetlić tylko jeden dysk twardy, gdy w monitorze sprzętu znajduje się wiele dysków twardych o tej samej nazwie.
* Zoptymalizowano efekt wyświetlania wykresu wykorzystania zasobów w trybie wykresu okna paska zadań.
* Zmień "Ustawienia wyświetlania" w menu kontekstowym okna paska zadań z menu na okno dialogowe.
* Dodano polecenie "Odśwież listę połączeń" do menu "Wybierz połączenia sieciowe".
* Naprawiono niektóre awarie.

## V1.82 (2021/12/12)

* Dodano nowy system wtyczek, możesz wyświetlić więcej dostosowanej zawartości w oknie paska zadań i oknie głównym za pomocą wtyczki.
* Dodano polecenie "Menedżer zadań" do menu kontekstowego okna paska zadań.
* Dodano przycisk "Zastosuj" do okna dialogowego ustawień opcji.
* Dodano widok tygodniowy w historycznych statystykach ruchu.
* Dodano ustawienie odstępu między elementami paska zadań.
* Naprawiono problem, że menu kontekstowe okna paska zadań nie mogło się wyświetlić w trybie ciemnym systemu Windows 11.
* Naprawiono problem, że okno paska zadań nie mogło zostać wyświetlone, gdy pasek zadań został przywrócony do stylu systemu Windows 10 za pomocą oprogramowania, takiego jak StartAllBack w systemie Windows 11.

## V1.81 (2021/07/27)

* Naprawiono kilka, awarii od wersji 1.80.
* Dodano obsługę niestandardowego sortowania elementów w oknie paska zadań.
* Gdy liczba elementów wyświetlanych w oknie paska zadań jest nieparzysta, ostatni element jest układany pionowo, aby zaoszczędzić miejsce na pasku zadań.
* Dodano funkcję monitorowania użycia dysku twardego.
* Dodano kilka ustawień w menu prawego przycisku myszy do okna dialogowego ustawień opcji.
* Dodano obsługę wielu monitorów okna głównego. Możesz przenieść okno główne na inne monitory bez zaznaczania opcji "Zezwalaj poza granice ekranu".

## V1.80.3 (2021/05/29)

* Naprawiono problem, przez który nie można było wyświetlić temperatury procesora
* Dodano funkcję wyboru temperatury określonego rdzenia procesora do monitorowania.
* Naprawiono problem, który powodował awarię programu.

## V1.80.2 (2021/05/22)

* Naprawiono kilka problemów, które powodowały awarię programu.
* Użyj LibreHardwareMonitor, aby uzyskać informacje o temperaturze.
* Dodano funkcję wyboru temperatury dysku twardego do monitorowania.
* Dodano funkcję wyboru sprzętu do monitorowania.
* Naprawiono problem, w którym plik LibreHardwareMonitorLib.sys nie jest zwalniany po zamknięciu programu.

## V1.80.1 (2021/05/17)

* Naprawiono problem, przez który nie można było uzyskać temperatury procesora graficznego AMD.
* Naprawiono problem, przez który program się zawieszał podczas automatycznego przełączania na ustawienie wstępne koloru trybu jasnego.
* Naprawiono problem, przez który nie można było uzyskać temperatury procesora Intel 11. generacji.
* Naprawiono problem, przez który ustawienie automatycznego uruchamiania nie działało.

## V1.80 (2021/05/15)

* Dodano funkcję monitorowania temperatury procesora, procesora graficznego, dysku twardego i płyty głównej.
* Dodano funkcję monitorowania wykorzystania procesora graficznego.
* Naprawiono problem, który powodował, że historyczne statystyki ruchu nie były poprawne z powodu przepełnienia danych.
* Naprawiono problem, który powodował, że pozycja głównego okna była niepoprawna po zmianie rozdzielczości ekranu.
* Naprawiono problem, który powodował, że rozmiar elementów interfejsu głównego okna i okna paska zadań nie zmieniał się po zmianie DPI systemu.
* Dodano możliwość swobodnego określania wyświetlanych elementów w oknie paska zadań.
* Dodano format xml głównego motywu okna, obsługujący wyświetlanie temperatury.
* Naprawiono problem, który powodował, że podokien w ustawieniach opcji nie można było przewijać za pomocą ekranu dotykowego podczas korzystania z urządzenia z ekranem dotykowym.
* Przeniesiono przetwarzanie sprawdzania aktualizacji do wątku w tle, aby rozwiązać problem, który powodował, że program przez długi czas nie reagował podczas sprawdzania aktualizacji.
* Naprawiono problem, który powodował, że program był nieprawidłowy po naciśnięciu Alt+F4 w oknie paska zadań.
* Dodano obsługę prędkości sieci powyżej 4 GB na sekundę.
* Dodano opcję ustawienia źródła aktualizacji jako Gitee, aby rozwiązać problem, że program czasami nie może być aktualizowany z powodu niedostępności GitHub w Chinach kontynentalnych.
* Dodano ustawienie trybu wyświetlania pamięci.
* Naprawiono problem, że aktualnie monitorowane połączenie sieciowe jest zamknięte lub wyłączone, poprzednio monitorowane połączenie sieciowe nie może być automatycznie wybrane po ponownym uruchomieniu.
* Automatyczne uruchamianie przy rozruchu jest realizowane przez Task Scheduler, aby rozwiązać problem, że automatyczne uruchamianie przy rozruchu czasami nie działa.
* Naprawiono niektóre awarie.
* Ulepszenia w innych szczegółach.

## V1.79.1 (2020/08/05)

* Naprawiono problem, wycieku uchwytu rejestru.
* Naprawiono problem, w którym użycie procesora i pamięci nie było liczone, gdy okno główne i okno paska zadań nie były wyświetlane.

## V1.79 (2020/07/30)

* Dodano funkcję automatycznego dostosowywania się do ciemnego/jasnego motywu okna paska zadań systemu Windows 10.
* Dodano funkcję automatycznego dostosowywania się do ciemnego/jasnego motywu ikony obszaru powiadomień systemu Windows 10.
* Dodano opcję metody pozyskiwania danych o użyciu procesora. Naprawiono problem, w którym użycie procesora może być zawsze równe 0 u niektórych użytkowników.
* Usuń opcję "Przezroczysty kolor" w "Opcje" – "Ustawienia okna paska zadań" i dodaj opcję "Przezroczysty kolor tła".
* Zezwala tylko na wyświetlanie użycia procesora i pamięci w oknie paska zadań bez szybkości sieci.
* Naprawiono problem, w którym ustawienia mogły nie zostać zapisane po wyłączeniu.
* Dodano ustawienie "pierwszego dnia tygodnia" w "Statystykach ruchu historycznego" – "Widok kalendarza".
* Dodano funkcję wyświetlania ruchu historycznego według dnia, miesiąca, kwartału i roku w "Statystykach ruchu historycznego" – "Widok listy".
* Naprawiono problem, utraty danych o ruchu historycznym z niewielkim prawdopodobieństwem.
* Naprawiono problem, przez który menu prawego przycisku myszy nie mogło się wyświetlić, jeśli tło okna paska zadań było ustawione na przezroczyste w jasnym motywie systemu Windows 10.
* Naprawiono problem, przez który statystyki ruchu nie obsługiwały danych przekraczających 2 TB.
* Dodano ikony dla elementów menu.
* Zapisano dane w katalogu AppData, gdy nie można zapisać katalogu programu.
* Dodano funkcję ustawiania interwału monitorowania.

## V1.78 (2020/03/21)

* Dodano funkcję otwierania określonej aplikacji poprzez dwukrotne kliknięcie okna głównego lub okna paska zadań.
* Dodano funkcję wyświetlania paska stanu wykorzystania procesora i pamięci w oknie paska zadań.
* Naprawiono niespójność wykorzystania procesora z menedżerem zadań w systemie Windows 10.
* Dodano opcję wyświetlania podpowiedzi dotyczących narzędzi myszy.
* Dodano funkcję ustawiania koloru paska zadań zgodnie z trybem oświetlenia systemu Windows 10 po pierwszym uruchomieniu programu.
* Dodano funkcję wstępnie ustawionych schematów w ustawieniach paska zadań.
* Ulepszenia w innych szczegółach.

## V1.77 (2019/05/01)
* Dodano ustawienie przezroczystego koloru okna zadań. Naprawiono problem polegający na tym, że kolor tekstu okna paska zadań nie mógł być ustawiony na czarny, gdy pasek zadań był biały (ustaw przezroczysty kolor w "Opcje" - "Ustawienia okna paska zadań").
* Dodano funkcję wyświetlania informacji o awarii, gdy program ulega awarii.
* Naprawiono problem, migotania ekranu podczas ponownego uruchamiania Eksploratora Windows, jeśli wyświetlane jest okno paska zadań.
* Wyświetlanie informacji o monitorowaniu, gdy mysz wskazuje ikonę obszaru powiadomień.
* Naprawiono problem, polegający na tym, że nie można było wyświetlić prędkości sieci podczas korzystania z sieci Bluetooth.
* Dodano wersję x64.
* Ulepszenia w innych szczegółach.
### Informacje o wydaniu:
Ta aktualizacja rozwiązuje problem, że tekst nie jest wyraźny na pasku zadań, gdy pasek zadań jest biały w najnowszej wersji systemu Windows 10. Musisz ręcznie ustawić kolor przezroczysty w "Opcje" - "Ustawienia okna paska zadań" - "Kolor przezroczysty". Jest to efekt nieprzezroczysty, gdy kolor przezroczysty nie jest czarny i różni się od koloru tła, a jest przezroczysty, jeśli jest taki sam jak kolor tła. Na białym pasku zadań systemu Windows 10 zaleca się ustawienie zgodnie z poniższym obrazkiem:
![白色任务栏截图](https://user-images.githubusercontent.com/30562462/57004858-36b55300-6c05-11e9-89d8-9911dc99f09c.PNG)
## V1.76 (2018/11/11)
* Naprawiono problem, który powodował awarię programu w wyniku zmiany rozdzielczości.
* Dodano opcje dotyczące lokalizacji zapisywania plików konfiguracyjnych.
* Naprawiono problem, który powodował, że pionowa pozycja okna paska zadań była nieprawidłowa po zmianie rozdzielczości.
* Dodano wyświetlanie dzisiejszego ruchu przesyłania i pobierania. Dodano statystyki ruchu przesyłania i pobierania w oknie dialogowym statycznych danych ruchu historii.
* Ulepszenia w innych szczegółach.
