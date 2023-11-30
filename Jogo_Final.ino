#include <SoftwareSerial.h>
SoftwareSerial bt(2,3);

bool conectado = false; //Funcionamento interno do jogo, serve para verificarmos se tem algum dispositivo conectado para poder jogar
String readbuff; //Local da memoria onde irá salvar o dado recebido pelo Bluetooth

int BTN_MODE = LOW; //Se o botão está em modo Terra ou Vcc

int LED_VERDE = 9, LED_VERMELHO = 10; //Pinos conectados
int BTN_VDD = 11, BTN_FALSO = 7; //Pinos conectados

bool pergunta = false; //Funcionamento interno do jogo, salva a veracidade da pergunta recebida

void callback(int pin); //Indicando ao compilador que  nos certificamos que este método será construido mais em baixo. 

struct button //Estrutura para verificar os botões, Já com proteção INPUT_PULLUP
{
   //Mesma logica da ultima aula
    bool state; 
    bool wasPressing;

    int pin;
	
  	void check() //Verifica se foi pressionado
    {
      if(pressed() && !wasPressing) // INICIO DO APERTO
      {
        wasPressing = true; //SALVE QUE APERTAMOS

        state = !state; //Altera o estado
        delay(200);
        callback(pin);
      }
      else if(!pressed() && wasPressing) //PARAMOS DE APERTAR?
      {
        wasPressing = false; //SALVE QUE SOLTAMOS
      }
    }
  
  	bool pressed()
    {
      return (digitalRead(pin) == BTN_MODE);
    }
  	
    button(int pin) //Construtor da estrutura
    {
      this->state = false;  //Configura valores padrão
      this->wasPressing = false;  //Configura valores padrão
      this->pin = pin;  //Configura valores definidos pela chamagem
      
      pinMode(pin, INPUT_PULLUP);
    };
};

button verdadeiro(BTN_VDD); //Botao verdadeiro
button falso(BTN_FALSO);  //Botao falso

void animar()  //Efeito visual de que o arduino ligou.
{
  digitalWrite(LED_VERDE, 1);
  delay(200);
  digitalWrite(LED_VERMELHO, 1);
  delay(200);
  digitalWrite(LED_VERDE, 0);
  delay(200);
  digitalWrite(LED_VERMELHO, 0);
  delay(200);
}

void setup() {
  Serial.begin(9600); //Inicia o Serial (Para debugarmos)
  bt.begin(9600); //Emula um Serial (Para seprar dados de debug do que será enviado para o Módulo Bluetooth)
  delay(500);
  
  Serial.println("Iniciando");

  //Configuração
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);  

  //Efeito visual de que o arduino ligou.
  animar();
}

//Tratar o dado recebido e processa-lo
void tratar(String dado)
{
  if(dado == "pronto" && !conectado) // se recebemos pronto mas n estamos conectados entao devemos conectar!
  {
    conectado = true;
    Serial.println("Conexão estabelecida.");
  }
  else if(dado == "desconectar" && conectado) // se recebemos um aviso q foi desconectado, entao desconectamos nossa logica para poder reconectar mais tarde sem precisar reiniciar o arduino
  {
    conectado = false;
    Serial.println("Fim de conexao via cliente.");
  }

  if(conectado) // Se estamos conectados as mensagens recebeidas serão parte do jogo
  {
    //Salva a veracidade da pergunta recebida.
    if(dado == "verdadeiro")
    {
      pergunta = true;
    }
    else if(dado == "falso")
    {
      pergunta = false;
    }
  }
}

//Receber dados do Bluetooth
void receber()
{
  while(bt.available() >0 ) //Prepara para tentar ler
  {
    delay(100); //Dar tempo para processar todos os dados
    char c = bt.read();
    readbuff+=c;
  }

  if (readbuff.length() >0) { //Tenta ler
    Serial.println("[DADO RECEBIDO]");
    Serial.println(readbuff); //Mostra o recebido
    readbuff.toLowerCase();
    tratar(readbuff);
    readbuff = ""; //limpa o texto para poder receber das proximas vezes sem interferencias
  }
}

void loop() 
{  
  receber(); //receber as mensagens
  
  if(conectado)
  {
    //Verifica se os botoes foram apertados.
    verdadeiro.check();
    falso.check();
  }
}

void callback(int pin)
{
  if(pin == BTN_VDD) //Callback para o botão verdadeiro
  {
    if(pergunta) //Se a pergunta é verdadeira e apertamos verdadeiro
    {
      Serial.println("[acertou] A pergunta é verdadeira, respondeu verdade.");
      bt.println("acertou");
      delay(300);
      digitalWrite(LED_VERDE, HIGH);
      delay(400);
      digitalWrite(LED_VERDE, LOW);
      Serial.println("Pedindo para gerar pergunta.");
      delay(400);
      bt.println("gerarpergunta");
      delay(400);
    }
    else //Se a pergunta é falsa e apertamos verdadeiro
    {
      Serial.println("[errou] A pergunta é falsa, respondeu verdade.");
      bt.println("errou");
      delay(300);
      digitalWrite(LED_VERMELHO, HIGH);
      delay(400);
      digitalWrite(LED_VERMELHO, LOW);
      bt.println("gerarpergunta");
      delay(400);
      Serial.println("Pedindo para gerar pergunta.");
      delay(400);
    }
  } 
  if(pin == BTN_FALSO)  //Callback para o botão falso
  {
    if(!pergunta) //Se a pergunta é falsa e apertamos falso
    {
      Serial.println("[acertou] A pergunta é falsa, respondeu falso.");
      bt.println("acertou");
      delay(300);
      digitalWrite(LED_VERDE, HIGH);
      delay(400);
      digitalWrite(LED_VERDE, LOW);
      bt.println("gerarpergunta");
      delay(400);
      Serial.println("Pedindo para gerar pergunta.");
      delay(400);
    }
    else //Se a pergunta é verdadeira e apertamos falso
    {
      Serial.println("[errou] A pergunta é verdadeira, respondeu falso.");
      bt.println("errou");
      delay(300);
      digitalWrite(LED_VERMELHO, HIGH);
      delay(400);
      digitalWrite(LED_VERMELHO, LOW);
      bt.println("gerarpergunta");
      delay(400);
      Serial.println("Pedindo para gerar pergunta.");
      delay(400);
    }
  } 
}
