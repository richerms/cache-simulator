#include <iostream>
#include <cstdlib>
#include <endian.h>
#include <arpa/inet.h>
#include <vector>
#include <list>

using namespace std;


class Blocos{
 	private:
 		int validade;
 		int tag;

 	public:
 		Blocos() {
 			this->validade = 0;
 			this->tag = -1;
 		}

 		void incrementaValidade() {
 			this->validade = 1;
 		}

 		void escreveTag(int tag) {
 			this->tag = tag;
 		}

 		int retornaValidade(){
 			return this->validade;
 		}

 		int retornaTag(){
 			return this->tag;
 		}
};

class Cache {
 	private:
	 	char politica;						// caractere da politica de substituição
 		int nSets;							// numero de conjuntos
 		int bSize;							// tamanho do bloco
 		int assoc;							// associatividade
		int tamCache;						// tamanho total da cache
		int totalAcessos;					// total de acessos a memoria
 		int hit;							// numero de hits
 		int missComp;						// quantidade de misses compulsorios
 		int missCapa;						// quantidade de misses de capacidade
		int missConf;						// quantidade de misses de conflito
		int totalMisses;					// somatorio do total de misses
		int tempTag;						// valor da tag do bloco temporario
		int tempIndice;						// valor do indice do bloco temporario
		int alocados;						// flag contando os endereços alocados na cache
		vector<class Blocos> vetorBlocos;	// vetor que armazena os blocos da cache

	public:
		Cache(int nSets = 256, int bSize = 4, int assoc = 1, char politica = 'R') {
			this->politica = politica;
			this-> nSets = nSets;
			this->bSize = bSize;
			this->assoc = assoc;
			this->tamCache = nSets * bSize * assoc;
			this->totalAcessos = 0;
			this->hit = 0;
			this->missComp = 0;
			this->missCapa = 0;
			this->missConf = 0;
			this->totalMisses = 0;
			this->tempTag = 0;
			this->tempIndice = 0;
			this->alocados = 0;
			this->vetorBlocos.reserve(nSets*assoc);
			for(int i=0; i<nSets*assoc; i++) {
				vetorBlocos[i] = Blocos();
			}
		}

		void incrementaAcessos() {
			this->totalAcessos++;
		}

		void incrementaCompulsorio() {
			this->missComp++;
		}

		void incrementaCapacidade() {
			this->missCapa++;
		}

		void incrementaConflito() {
			this->missConf++;
		}

		void incrementaAcerto() {
			this->hit++;
		}

		void incrementaAlocados() {
			this->alocados++;
		}

		int retornaTamCache(){
			return this->tamCache;
		}

		void somaTotalMisses() {
			this->totalMisses = this->missComp + this->missCapa + this->missConf;
		}

		void imprime(int flag){
			// se a flag de saída for 0, imprime o modelo livre
			if (flag == 0){
			cout << fixed;
			cout.precision(2);
			cout << "Total de acessos:             " << this->totalAcessos << endl;
			cout << "Taxa de hits:                 " << (float)this->hit/totalAcessos << endl;
			cout << "Taxa de misses:               " << (float)this->totalMisses/totalAcessos << endl;
			cout << "Taxa de misses compulsórios:  " << (float)this->missComp/totalMisses << endl;
			cout << "Taxa de misses de capacidade: " << (float)this->missCapa/totalMisses << endl;
			cout << "Taxa de misses de conflito:   " << (float)this->missConf/totalMisses << endl;
			}
			// caso contrário, o modelo padrão será impresso na tela
			else{
				cout << fixed;
				cout.precision(2);
				cout << this->totalAcessos << " "
				<< (float)this->hit/totalAcessos  << " "
				<< (float)this->totalMisses/totalAcessos << " "
				<< (float)this->missComp/totalMisses << " "
				<< (float)this->missCapa/totalMisses << " "
				<< (float)this->missConf/totalMisses  << endl;
			}
		}

		void acessaCache(int endereco){
			incrementaAcessos();
			this->tempTag = (endereco/this->bSize)/this->nSets;
			this->tempIndice = (endereco/this->bSize) % this->nSets;
			int n; // variavel laço for
			int i = this->tempIndice*this->assoc;
			int espacoVazio = -1; // representa o valor do vazio
			for(n=i; n<i+assoc; n++) {
				if(vetorBlocos[n].retornaTag() == this->tempTag){ // hit
					incrementaAcerto();
					return;
				}
				if(espacoVazio==-1){ // inicia a procura pelo primeiro espaco vazio
					if(vetorBlocos[n].retornaValidade()==0){
						espacoVazio = n;
					}
				}
			}
			// Identifica os Misses

			// Miss Capacidade
			if(this->alocados >= (this->nSets*this->assoc)) {
				incrementaCapacidade();
				vetorBlocos[this->tempIndice*this->assoc+(rand()%assoc)].escreveTag(this->tempTag);
			} 
			// Miss Compulsorio
			else if(espacoVazio!=-1){
				incrementaCompulsorio();
				vetorBlocos[espacoVazio].incrementaValidade();
				vetorBlocos[espacoVazio].escreveTag(this->tempTag);
				incrementaAlocados();
			} 
			// Miss Conflito
			else {
				incrementaConflito();
				vetorBlocos[this->tempIndice*this->assoc+(rand()%assoc)].escreveTag(this->tempTag);
			}
		somaTotalMisses();
		}
};

// Faz a leitura do arquivo e armazena os dados em f
FILE* leArquivo(char* nomeArquivo){
	FILE *f = fopen(nomeArquivo, "r");
	return f;
}

// Le cada dado e enquanto nao chegar no End Of File, acessa a Cache com o dado lido
void percorreCache(Cache * novaCache, FILE *f){
	int end;
	while(!feof(f)){
		fread(&end, sizeof(int), 1, f);
		if(!feof(f)){
			end = ntohl(end);
			novaCache->acessaCache(end);
		}
	}
}

int main(int argc, char* argv[]) {

	// executa o simulador com os parâmetros da cache enviados na execução
	if (argc == 7){
		Cache *novaCache = new Cache(stoi(argv[1]),stoi(argv[2]),stoi(argv[3]), argv[4][0]);
		FILE *f = leArquivo(argv[6]);
		percorreCache(novaCache, f);
		novaCache->imprime(stoi(argv[5]));
		return 0;
	}	
	// caso não hajam parâmetros para a cache e flag, uma cache padrão
	// está pré-definida como 256:4:1:r e será impressa no formato padrão
	else if (argc == 2){
		Cache *novaCache = new Cache();
		FILE *f = leArquivo(argv[1]);
		percorreCache(novaCache, f);
		novaCache->imprime(1);
		return 0;
	}
	// se nenhuma das opções acima for atendida, retorna a frase a seguir
	else
	{
		cout << "Confira seus parametros e execute novamente!" << endl;
		return 0;
	}
	

}
