#include <iostream>
#include <fstream>
using namespace std;

ifstream f("BTree.in");
ofstream g("BTree.out");

/// ------------------------------------------ clasa Node
class Node{
    int *valori;
    int nr_valori;
    Node **children;
    int grad_minim;
    bool frunza;
public:
    Node(int grad_minim, bool frunza){
        this->grad_minim = grad_minim;
        this->frunza = frunza;
        valori = new int [2*grad_minim - 1];
        children = new Node *[2*grad_minim];
        this->nr_valori = 0;
    }

/// ------------------------------------------------------------------------------functia care imparte un nod in doi frati

    void split(int pozitie_frate_stang, Node *frate_stang){
        Node *frate_drept = new Node(frate_stang->grad_minim,frate_stang->frunza);
        /// -----------------------------
        /// copiem ulimele grad_minim - 1 chei din frate_stang in frate_drept
        frate_drept->nr_valori = grad_minim - 1;
        for(int j = 0; j < grad_minim - 1; j++){
            frate_drept->valori[j] = frate_stang->valori[j+grad_minim];
        }
        /// copiem si ultimii grad_minim copii ai fratelui stang in copiii fratelui drept daca nu este nod intern(adica frunza == false)
        if(frate_stang->frunza == false){
            for(int j = 0; j < grad_minim; j++){
                frate_drept->children[j] = frate_stang->children[j+grad_minim];
            }
        }
        /// numarul de chei din fratele stang trebuie sa fie acum grad_minim - 1
        frate_stang->nr_valori = grad_minim - 1;

        for(int j = this->nr_valori; j >= pozitie_frate_stang + 1; j--)
            this->children[j+1] = this->children[j];

        children[pozitie_frate_stang+1] = frate_drept;
        for(int j = nr_valori - 1; j >= pozitie_frate_stang; j--)
            this->valori[j+1] = this->valori[j];

        this->valori[pozitie_frate_stang] = frate_stang->valori[grad_minim - 1]; /// punem pe pozitie_frate_stang in vectorul de valori al parintelui valoarea mediana din frate_stang
        this->nr_valori++;
    }

/// -------------------------------------------------------------- functie de inserare

    void insereaza_undeva(int valoare){
        /// i este indexul celei mai din dreapta valori din nodul care a apelat functia
        int i = nr_valori - 1;
        if(this->frunza == true){
            ///cautam locul de insertie a valorii
            while(i>=0 and valori[i] > valoare){
                valori[i+1] = valori[i];
                i--;
            }
            valori[i+1] = valoare;
            nr_valori++;
        }
        else{
            /// cautam pozitia copilului unde trebuie inserata valoarea
            while(i>=0 and valori[i] > valoare){
                i--;
            }

            if(children[i+1]->nr_valori == 2*grad_minim - 1){
                this->split(i+1, children[i+1]);
                if(valori[i+1] < valoare)
                    i++;
            }
            children[i+1]->insereaza_undeva(valoare);
        }
    }

/// -------------------------------------------------------------- functie de parcurgere a unui nod(apelata recursiv de copiii nodului)

    void parcurge(){
        int i;
        /*cout<<nr_valori<<" ";{
            for(int j = 0; j<nr_valori;j++)
               cout<<valori[j]<<" ";
            cout<<endl;
        }*/

        for(i = 0; i < nr_valori; i++){

            if(this->frunza == false)
                children[i]->parcurge();
            g<<valori[i]<<" ";
        }

        if(this->frunza == false)
            children[i]->parcurge();
    }

/// ---------------------------------------------------------------- functie de cautare a unei valori in nod

    Node* cauta(int valoare){
        int i = 0;
        while(i < nr_valori && valoare > valori[i])
            i++;
        if(valori[i] == valoare)
            return this;
        if(frunza == true)
            return nullptr;
        return children[i]->cauta(valoare);
    }

/// ----------------------------------------------------------------------functie care returneaza indexul valorii cautate sau a celei imediat dupa

    int gaseste_index(int valoare){
        int i=0;
        while(i<nr_valori and valori[i] < valoare)
            i++;
        return i;
    }

/// ----------------------------------------------------------functia de stergere din clasa Node
    void sterge(int valoare){
        int index = this->gaseste_index(valoare);

        if(index < nr_valori and valori[index] == valoare){
            if(this->frunza == true)
                sterge_din_frunza(index);
            else
                sterge_din_nod_intern(index);
        }
        else{
            if(this->frunza == true){
                g << "Elementul "<<valoare<<" nu exista in B-Arbore.\n";
                return;
            }

            bool verif = false;
            if(index == nr_valori) verif = true;

            if(children[index]->nr_valori < grad_minim)
                this->completeaza(index);

            if(verif and index > nr_valori) index--;
            children[index]->sterge(valoare);
        }
        return;
    }
/// ----------------------------------------------------functia de stergere din frunza
    void sterge_din_frunza(int index){
        for (int i = index + 1; i < nr_valori; i++)
            valori[i-1] = valori[i];
        nr_valori--;
        return;
    }
/// -----------------------------------------------------functia de stergere dintr-un nod intern
    void sterge_din_nod_intern(int index){

        int valoare = valori[index];
        if(children[index]->nr_valori >= grad_minim){
            int predecesor = cauta_predecesor(index);
            valori[index] = predecesor;
            children[index]->sterge(predecesor);
        }
        else if(children[index+1]->nr_valori >= grad_minim){
            int succesor = cauta_succesor(index);
            valori[index] = succesor;
            children[index+1]->sterge(succesor);
        }
        else{
            concateneaza(index);
            children[index]->sterge(valoare);
        }
        return;
    }

/// ---------------------------------------------------------------functie de gasire a predecesorului
    int cauta_predecesor(int index){
        Node *nod = children[index];
        while(nod->frunza == false)
            nod = nod->children[nod->nr_valori];

        return nod->valori[nod->nr_valori-1];
    }
/// ----------------------------------------------------------------functie de gasire a succesorului
    int cauta_succesor(int index){
        Node *nod = children[index+1];
        while(nod->frunza == false)
            nod = nod->children[0];
        return nod->valori[0];
    }

/// -----------------------------------------------------------------functia de completare
    void completeaza(int index){
        if(index != 0 and children[index-1]->nr_valori >= grad_minim)
            ia_din_stanga(index);
        else if(index != nr_valori and children[index+1]->nr_valori >= grad_minim)
            ia_din_dreapta(index);
        else{
            if(index != nr_valori) concateneaza(index);
            else concateneaza(index-1);
        }
        return;
    }
/// -------------------------------------------------------------------functia de imprumut a unei valori de la fratele din stanga
    void ia_din_stanga(int index){

        Node *copil = children[index];
        Node *frate = children[index-1];
        for (int i = copil->nr_valori-1; i >= 0; i--)
            copil->valori[i+1] = copil->valori[i];

        if(copil->frunza == false){
            for(int i = copil->nr_valori; i>=0; i--)
                copil->children[i+1] = copil->children[i];
        }

        copil->valori[0] = valori[index-1];

        if(copil->frunza == false)
            copil->children[0] = frate->children[frate->nr_valori];

        valori[index-1] = frate->valori[frate->nr_valori-1];

        copil->nr_valori += 1;
        frate->nr_valori -= 1;
        return;
    }

/// ------------------------------------------------------ functie de imprumut a unei valori de la fratele din dreapta
    void ia_din_dreapta(int index){

        Node *copil = children[index];
        Node *frate = children[index+1];

        copil->valori[(copil->nr_valori)] = valori[index];

        if(copil->frunza == false)
            copil->children[(copil->nr_valori)+1] = frate->children[0];

        valori[index] = frate->valori[0];

        for(int i = 1; i < frate->nr_valori; i++)
            frate->valori[i-1] = frate->valori[i];

        if(frate->frunza == false){
            for(int i = 1; i <= frate->nr_valori; i++)
                frate->children[i-1] = frate->children[i];
        }

        copil->nr_valori += 1;
        frate->nr_valori -= 1;
        return;
    }

/// -------------------------------------------------------functia de concatenare
    void concateneaza(int index){
        Node *copil = children[index];
        Node *frate = children[index+1];

        copil->valori[grad_minim-1] = valori[index];

        for(int i = 0; i < frate->nr_valori; i++)
            copil->valori[i+grad_minim] = frate->valori[i];

        if(copil->frunza == false){
            for(int i = 0; i <= frate->nr_valori; i++)
                copil->children[i+grad_minim] = frate->children[i];
        }

        for(int i = index + 1; i < nr_valori; i++)
            valori[i-1] = valori[i];

        for(int i = index + 2; i <= nr_valori; i++)
            children[i-1] = children[i];

        copil->nr_valori += frate->nr_valori+1;
        nr_valori--;

        delete frate;
        return;
    }

    friend class BTree;
};

/// --------------------------------------------------------------------------------- clasa BTree
class BTree{
    Node *radacina;
    int grad_minim_btree;
public:
    BTree(int grad_minim_btree){
        this->radacina = nullptr;
        this->grad_minim_btree = grad_minim_btree;
    }
///----------------------------------------------------------------------------------functia de parcurgere
    void parcurge(){
        if(radacina != nullptr)
            radacina->parcurge();
    }

///-----------------------------------------------------------------------------------functia de inserare
    void insereaza(int valoare){
        if(radacina == nullptr){
            radacina = new Node(grad_minim_btree, true);  /// construim un nod ca radacina si va fi frunza initial
            radacina->valori[0] = valoare;
            radacina->nr_valori = 1;
        }
        else if(radacina->nr_valori == 2*grad_minim_btree - 1){
            Node *noua_radacina = new Node(grad_minim_btree,false); /// noua radacina
            noua_radacina->children[0] = radacina;   /// vechea radacina devine copil pe care il bifurcam in frati
            noua_radacina->split(0,radacina);
            /// ----------------------------------------
            int i = 0;
            if(noua_radacina->valori[0] < valoare){
                i++;
            }
            noua_radacina->children[i]->insereaza_undeva(valoare);
            radacina = noua_radacina; /// arborele a crescut in inaltime si vechea radacina ia valoarea noii radacini
        }
        else{
            radacina->insereaza_undeva(valoare);
        }
    }

///------------------------------------------------------------------------------------- functia de cautare

    Node* cauta(int valoare){
        if(radacina == nullptr) return nullptr;
        else return radacina->cauta(valoare);
    }

///-------------------------------------------------------------------------------------- functie de stergere

    void sterge(int valoare){
        if(radacina == nullptr){
            g << "Arbore vid.\n";
            return;
        }

        radacina->sterge(valoare);

        if(radacina->nr_valori == 0){
            Node *radacina_veche = radacina;
            if(radacina->frunza == true)
                radacina = nullptr;
            else
                radacina = radacina->children[0];
            delete radacina_veche;
        }
        return;
    }
};

/// ------------------------------------------------------------------- MAIN
int main(){

    int grad;
    f>>grad;
    BTree arb(grad);

    int valoare;
    while(f>>valoare)
        arb.insereaza(valoare);

    ///arb.traverseaza();
    ///if(arb.cauta(78) != nullptr) g<<"\nS-a gasit!";
    ///else g<<"\nNu s-a gasit!";
    ///arb.parcurge(); g<<'\n';
    ///1 2 0 3 9 1 4 6 2 10 5
    /*arb.sterge(1);
    arb.sterge(2);
    arb.sterge(0);
    arb.sterge(3);
    arb.sterge(9);
    arb.sterge(1);
    arb.sterge(4);
    arb.sterge(6);
    arb.sterge(2);
    arb.sterge(10);*/
    ///arb.sterge(5);
    ///arb.parcurge();
    ///arb.sterge(14);
    ///arb.sterge(23);
    ///arb.parcurge();
    return 0;
}
