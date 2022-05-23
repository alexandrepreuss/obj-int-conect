# Domótica aplicada ao controle de reservatórios de água em residência rural

Autores: **Alexandre Ricci Preuss, William Vilas Boas Gomes**

Professor Orientador: **Wallace Rodrigues de Santana**

---

_**Resumo:** Este repositório descreve o projeto em escala de um sistema para controle de dois reservatórios de água independentes, verificando as variações nos níveis de água e automatizando comandos para movimentação de água baseados critérios pré-estabelecidos, utilizando arduino._

![](src/img/esquema-de-abastecimento.jpg)

**Figura 1 - Esquema de abastecimento de água do projeto proposto**

![](src/img/prototipo.jpg)

**Figura 2 - Protótipo em escala utilizado**

---

## Configuração

### Materiais necessários:
- 01 Placa Arduino Uno
- 01 Protoboard
- 01 Bomba de aquário
- 01 Relé (SRD-05VDC-SL-C)
- 01 Led Vermelho
- 02 Leds Azuis
- 02 Leds Amarelos
- 02 Sensores Ultrassônicos (HC-SR04)
- 05 Resistores (1kΩ)
- Cabos jumpers para a conexão do materiais à placa e à protoboard

![](src/img/configuracao.jpg)

**Figura 3 - Configuração**

---

## Utilizando o código

Para a utilização do código, é necessária a instação do do software Arduino IDE. Após instalado, importar o código em Arquivo > Abrir..., ou utilizando o atalho CTRL + O.

Selecionar o arquivo 'monitoring-system.ino', dentro da pasta 'src/monitoring-system' e carregar na placa Arduino.

## Funcionamento
### Sistema

O sistema realiza leitura com seus sensores ultrassônicos, sendo um sensor para o reservatório de baixo e o outro para o de cima, converte os valores de leitura em centímetro e os armazena e em variáveis. Essas variáveis são analisadas no código, comparando-as com os valores de níveis mínimo e máximo (variáveis descritas no código) de seus respectivos reservatórios.

Caso o sensor do reservatório de baixo apresente uma leitura onde represente um nível de água acima do nível mínimo e o sensor do reservatório de cima apresente uma leitura representando um nível abaixo do máximo, é acionado o relé e com isso, o funcionamento da bomba é ativado, tranferindo a água do reservatório de baixo para o de cima.

O funcionamento da bomba é interrompido automaticamente em duas situações:

- caso o sensor de baixo realize uma leitura que iguale ou ultrapasse o nível mínimo do reservatório, prevenindo o esvaziamento do volume morto, necessário para o funcionamento da bomba;
- caso o sensor de cima realize uma leitura que iguale ou ultrapasse o nível máximo do reservatório, prevenindo o vazamento de água.

Existe também a possibilidde de um controle via internet, com a utilização do protocolo MQTT. Porém, há uma verificação no código para esse controle, para prevenir que ocorra algum dos riscos citados acima, com o acionamento indevido da bomba.

Os dados gerados a cada ciclo de código são enviados para um servidor node-red, onde esses dados são analizados e atualizam a uma dashboard de controle do projeto.

---

### Node-RED
O Node-RED é uma ferramenta de programação baseada em fluxo (_flow_), originalmente desenvolvida pela IBM e atualmente faz parte da OpenJS Foundation.
A instalação do Node-RED é feita via npm (_Node Package Manager_), o gerenciador de pacotes do Node.js.
Inicialmente, o Node-RED disponibiliza algumas paletas básicas de nós (_nodes_), com os quais podemos criar de forma simples o fluxo, arrastando e conectando uns aos outros. Alguns nós permitem a programação via JavaScript. Além dos nós disponibilizados, podemos também adicionar diversas novas paletas e bibliotecas, como por exemplo, nós que possibilitam a utilização de dashboards. A criação e edição dos fluxos é realizada em qualquer navegador, após inicializar o Node-RED via terminal.

Para mais informações, acesse a [documentação do Node-RED](https://nodered.org/docs/)

![](src/img/fluxo-node-red.jpg)

**Figura 4 - Fluxos**

**Obs:** Para a utilização do fluxo acima são necessários os nós 'node-red-node-serialport' e 'node-red-dashboard', que podem ser instalados via node-red na opção Manage palette.

![](src/img/no-caixa-baixo.jpg)

**Figura 5 - Código do nó function ligado ao tópico mack-2022-obj-int-conect/tanks/bottom**

![](src/img/no-caixa-cima.jpg)

**Figura 6 - Código do nó function ligado ao tópico mack-2022-obj-int-conect/tanks/upper**

![](src/img/no-bomba.jpg)

**Figura 7 - Código do nó function ligado ao tópico mack-2022-obj-int-conect/pump**

![](src/img/no-functions.jpg)

**Figura 8 - Código dos demais nós functions**

![](src/img/no-change.jpg)

**Figura 9 - Configuração do nó change**

---

### Protocolo MQTT

O MQTT é um de protocolo de mensagens leves muito utilizado em Internet das Coisas (IoT - Internet of Things). 

Sua comunicação é feita via tópicos, nos quais é possível publicar ou se inscrever, mediante a utilização servidor broker

Para mais informações, acesse a [documentação do MQTT](https://mqtt.org/)

Foi utilizado no projeto o [HiveMQ](https://mqtt.org/), um broker público e gratuito, que também possui um _Client_, no qual é possível publicar e se inscrever em tópicos, possibilitando a conexão via internet.