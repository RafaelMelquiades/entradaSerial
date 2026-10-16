#include <Arduino.h>

int lerProximoNumero(char*& cursor);

void leituraBloqueante();
void leituraNaoBloqueante();
void processaFrase(char* texto);
void aplicaCor();

void setup() 
{
  Serial.begin(115200);
  Serial.setTimeout(2000);
  // readStringUntil espera no maximo por caractere esse tempo(ms) pelo '\n'
  // o padrao e 1000ms

  Serial.println();
  Serial.println("Digite o seu nome e pressione ENTER:");

}

int vermelho = 0;
int verde = 0;
int azul = 0;
int brilho = 0;

void loop() 
{
  leituraNaoBloqueante();
}

void leituraNaoBloqueante()
{
  const int TAMANHO_MAX = 50; // tamanho do vetor(incluir '/0').
  static char frase[TAMANHO_MAX];
  static int position = 0;

  while(Serial.available() > 0)
  {
    char c = Serial.read();

    if(c == '\r') continue;
    // o continue pula o restante do while naquela volta e passa pra leitura do proximo caractere.
    // o '\r' é lido e simplesmente descartado, sem entrar no vetor.

    if(c == '\n')
    {
      frase[position] = '\0';
      if(position > 0) processaFrase(frase);
      position = 0;
    }

    else if(position < TAMANHO_MAX - 1)
    {
      frase[position] = c;
      position++;
    }
    
  }

}

// vermelho, verde, azul, branco, desliga.
// rgb r g b (ex.: rgb 200 125 98)
// brilho n (0 a 100%)
void processaFrase(char* texto)
{
  Serial.printf("Recebido: [%s]\n\r", texto);

  char* comando = texto;
  char* parametro = NULL; // NULL = \0

  char* espaco = strchr(texto, ' ');

  if(espaco != NULL) // se ele encontrou um espaco...
  {
    parametro = espaco + 1;
    *espaco = '\0';
  }


  if(strcasecmp(comando, "VERMELHO")==0)
  {
    vermelho = 255;
    verde = 0;
    azul = 0;
    brilho = 100;
  }

  else if(strcasecmp(comando, "VERDE")==0)
  {
    vermelho = 0;
    verde = 255;
    azul = 0;
    brilho = 100;
  }

  else if(strcasecmp(comando,"AZUL")==0)
  {
    vermelho = 0;
    verde = 0;
    azul = 255;
    brilho = 100;
  }

  else if(strcasecmp(comando, "BRANCO")==0)
  {
    vermelho = 255;
    verde = 255;
    azul = 255;
    brilho = 100;
  }

  else if(strcasecmp(comando, "DESLIGAR")==0)
  {
    vermelho = 0;
    verde = 0;
    azul = 0;
    brilho = 0;
  }

  else if(strcasecmp(comando, "RGB")==0)
  {
    if(parametro == NULL)
    {
      Serial.println("Use o comando: RGB r g b (0 a 255)");
      return;
    }
    
    char* cursor = parametro;

    int r = lerProximoNumero(cursor);
    int g = lerProximoNumero(cursor);
    int b = lerProximoNumero(cursor);
    
    vermelho = constrain(r, 0, 255); // limita o valor ja convertido para int de 0 a 255;
    verde = constrain(g, 0, 255);
    azul = constrain(b, 0, 255);
    
  }

  else if(strcasecmp(comando, "BRILHO")==0)
  {
    if(parametro == NULL)
    {
      Serial.println("Use o comando: BRILHO n (0 a 100)");
      return;
    }

    brilho = constrain(atoi(parametro), 0, 100);
  }

  else
  {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Comando: [%s] não existe", comando);
    Serial.println(buffer);
    return;
  }
  aplicaCor();
}

void aplicaCor()
{
  int r = vermelho * brilho / 100;
  int g = verde * brilho / 100;
  int b = azul * brilho / 100;

  rgbLedWrite(PIN_RGB_LED, r, g, b); // controla o led rgb, funcao nova.

  char bufferMensagem[80];
  snprintf(bufferMensagem, sizeof(bufferMensagem), "Cor: R = %3d G = %3d B = %3d | Brilho: %3d %% | HEX: #%02X%02X%02X", 
            vermelho, verde, azul, brilho, r, g, b);

  Serial.println(bufferMensagem);
}
  

//r g b
// cursor é uma referencia (&) para um ponteiro char (char*).
// a funcao recebe a propria variavel de quem chamou e nao uma copia.
//
/*
    PARAMETRO   |   ALTERA O TEXTO APONTADO   |  ALTERA PARA ONDE O PONTEIRO DE FORA APONTA
  char * p      |  sim(*p = '\0')             | nao, pq ele e uma copia
  const char* p |  nao                        | nao
  char* &p      |  sim                        |

*/
int lerProximoNumero(char*& cursor)
{
  if(cursor == NULL) return 0; // protege o estouro de memoria no caso atoi(NULL).
  int valor = atoi(cursor);
  char* espaco = strchr(cursor, ' ');
  cursor = (espaco != NULL) ? espaco + 1 : NULL;
  return valor;
}

void leituraBloqueante()
{
  // available() diz quantos bytes ja chegaram e estao esperando leitura.
  if(Serial.available() > 0)
  {
    String texto = Serial.readStringUntil('\n');

    // trim() remove espacos,'/r', '\t', '\n'que alguns terminais enviam.
    texto.trim();

    if(texto.length() == 0) return;

    Serial.printf("Seu nome é %s\n\r", texto.c_str());
    Serial.printf("Quantidade de caracteres: %u \n\r", texto.length());

    Serial.printf("Primeira letra: %c\n\r", texto.charAt(0));
    Serial.printf("Ultima letra: %c\n\r", texto[texto.length() - 1]);

    String maiusculo = texto;
    maiusculo.toUpperCase();
    Serial.printf("Em maiusculas: %s\n\r", maiusculo.c_str());

    if(maiusculo == "RAFAEL")
    {
      Serial.println("Este é seu nome correto");
    }
  }
}