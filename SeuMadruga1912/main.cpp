#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int main()
{
    int n;
    double a;
    while(1){

        cin >> n >> a;
        if(n == 0 && a == 0)break;
        double total = (double)0.0;
        double aux;
        std::vector<double> fita;

        //For para preenchimento do n
        for(int i=0;i<n;i++){
            cin >> aux;
            fita.push_back(aux);
            total += (double) aux;
        }
        // Encontrando o maior elemento usando max_element
        auto maxElement = std::max_element(fita.begin(), fita.end());

        if(total < a){
            printf("-.-\n");
        }else if(total == a){
            printf(":D\n");
        }else{
            double meio, inicio = 0, fim = *maxElement;
            double precisao = total;
            while(1){
                double cal=0.0;
                double tolerance = 0.0001;
                meio = (inicio + fim)/2.0;
                for (size_t i = 0; i < fita.size(); i++) {
                    if(fita[i] > meio){
                        cal+=meio;
                    }else{
                        cal += fita[i];
                    }
                }
                if(std::abs(total - cal - a) < tolerance){
                    printf("%.4lf\n",meio);
                    break;
                }else if(total - cal > a){
                    inicio = meio;
                }else{
                    fim = meio;
                }
            }

        }
    }
    return 0;
}
