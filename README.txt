TEMA 2 - Diaconu Maria-Loredana, 325CA

In aceasta tema am utilizat scheletul de la laboratoarul 10.
Mesajele trimise catre server sunt compuse cu ajutorul functiilor din requests.cpp
(compute_get_request, compute_post_request), pe care le-am modificat, adaugand camp
pentru tokens si pastrand un singur cookie in loc de array de cookies. Am adaugat si
functia de compute_delete_request, asemanatoare cu requestul de GET.

Chiar daca tema este in C++, am ales sa folosesc parson pentru simplul fapt ca avea
documentatia mai scurta si mai la obiect decat nlohmann si avea un exemplu in README
care se preta perfect pe ce aveam nevoie :) 

I. client.cpp
   Aici se citesc comenzi de la tastatura pana la intalnirea comenzii "exit"
   si se executa, apeland functiile corespunzatoare. Ordinea actiunilor este
   asemanatoare la fiecare: 
       * Se verifica o situatie care poate duce la o eroare evidenta 
       * Se primesc date de la utilizator (daca este cazul)
       * Se deschide conexiunea cu serverul, se compune un mesaj pentru server
         si se trimite.
       * Se primeste un raspuns in care se verifica eventualele erori si, daca
         nu exista, se extrag informatiile necesare si se afiseaza mesajele
         corespunzatoare.
    Particular, pentru fiecare comanda:
        1. execute_register
           Nu se poate inregistra un cont nou daca este cineva deja logat,
           utilizatorul este rugat sa dea log out.
           Altfel, se citesc usernameul si parola de la tastatura, se compune
           mesajul care va fi trimis serverului ce contine aceste date, ca obiecte
           JSON.
           Se cauta eroare in raspunsul de la server cu ajutorul functiei
           error_found si se afiseaza daca actiunea a fost executata cu succes
           (usernameul nu exista deja).
        2. execute_login
           Utlizatorul nu se poate loga daca este deja cineva logat.
           Restul este asemanator cu execute_register. Eroarea de la server se
           refera la faptul ca usernameul sau parola sunt gresite.
           Daca actiunea este realizata cu succes, se cauta in raspuns cookie-ul
           de login si se retine.
        3. enter_library
           Se poate realiza doar daca este cineva logat (sirul login_cookie nu
           este gol). Se cauta erori in raspunsul de la server. Daca nu exista,
           este retinut si tokenul ce demonstreaza accesul la biblioteca.
        4. get_books
           Se poate realiza daca utilizatorul are acces la biblioteca (sirul
           access_token nu este gol). Daca nu exista nicio eroare in raspuns, 
           se cauta lista de obiecte json si se afiseaza.
        5. get_book
           Aici erorile pot proveni de la lipsa accesului la biblioteca sau
           cererea pe un id care nu exista (nu am considerat necesara verificarea
           daca id nu este un intreg, deoarece acest caz duce tot la eroarea "Nu 
           exista acest id"). Pentru mesaju de GET, am compus ruta de acces, adaugand
           "/id" (id fiind id-ul cartii) la finalul sirului "/api/v1/tema/library/books".
        6. add_book
           Este verificat accesul la biblioteca. Utilizatorul trebuie sa introduca
           detalii despre carte. Daca introduce un sir ce nu reprezinta un intreg la
           page_count, este instiintat. Se cere page_count pana cand utilizatorul
           introduce un sir valid.
        7. delete_book
           Erorile pot proveni de la lipsa accesului sau de la introducerea unui id
           ce nu exitsa. Mesajul catre server este compus cu ajutorul functiei
           compute_delete_request din requests.cpp.
        8. execute_logout
           Se poate executa doar daca utilizatorul este logat. Se sterg informatiile
           legate de cookieul de login si tokenul de acces. Astfel, biblioteca nu mai
           poate fi accesata dupa ce utilizatorul da log out. 

II. helpers.cpp
    Era deja prezent in scheletul de laborator. Am adaugat defineuri si functii
    ajutatoare.
    1. find_json
       Gaseste lista de obiecte JSON dintr-un sir. Este utilizata in get_book(s)
       pentru a extrage lista de carti sau informatiile despre o carte din
       raspunsul serverului.
    2. parse_usr_pswd
       Intoarce usernameul si parola ca lista de obiecte JSON.
    3. parse_book_to_json
       Intoarce informatiile despre o carte ca lista de obiecte JSON.
    4. error_found
       Cauta un mesaj de eroare in raspunsul de la server. Intoarce true daca exita.
    5. get_cookie
       Extrage cookieul din raspunsul serverului.
    6. get_token
       Extrage tokenul din raspunsul serverului.