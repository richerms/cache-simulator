# cache-simulator
Simulador de Cache desenvolvido para a disciplina de Arquitetura e Organização de Computador II do Curso de Ciência da Computação da UFPel no semestre 2019/2.

```

Necessário apenas o G++:
sudo apt install g++

Comando para criar o executavel:
g++ -std=c++11 cache_simulator.cpp -o cache_simulator

Comando para executar:
Com parâmetros:
	./cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada
Ex.:
	./cache_simulator 256 4 1 R 1 bin_100.bin

Sem parâmetros:
	./cache_simulator arquivo_de_entrada
Ex.:
	./cache_simulator bin_100.bin

Não há funcionalidades adicionais(bônus)

Mais detalhes nos comentários do código e no arquivo "relatorio.pdf"

```
