//_____  I N C L U D E S ___________________________________________________


#include <config.h>
#include "print_command_task.h"
#include "cislb00p65-print_barcode.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <matrizes_fontes.h>
/***  DECLARAÇÃO DE VARIÁVEL GLOBAL ***/
#define VERSION_IE

typedef struct st_prtcmd_matriz_param {
  unsigned char col_ref;
  unsigned char linha_ref;
  unsigned char linhas_antes;
  unsigned char linhas_depois;
} st_prtcmd_matriz_param;

uint16_t  libv_prtcmd_MsgSize;

char CommandPending[2] = { 0, 0};
unsigned char libv_canal_print_command;
uint16_t libv_prtcmd_rascunho_nbytes = 0;
uint8_t libv_prtcmd_repete = 0;
uint8_t libv_prtcmd_nlinha = 0;
uint8_t libv_prtcmd_rascunho[32];
uint8_t libv_rascunho_byte_grafico[384];
uint8_t libv_rascunho_byte_grafico_out[10];
uint16_t libv_prtcm_ctl_bytes = 0;
uint8_t  Barras[384];
uint8_t  BarraPreta[384];
uint8_t  libv_altura_grafico_bits; /*uint8_t*/
uint8_t libv_byte_grafico_auxiliar; /*uint8_t*/
uint16_t libv_quantidade_bytes_grafico; /*uint16_t*/
uint16_t i,j; /*uint8_t*/
uint8_t filletBuffer[386];

char lib_prtcmd_get_matriz(unsigned short posic_char, unsigned char *tab_matriz, unsigned char *matriz_out, unsigned char matriz_size, uint8_t num_cols, uint8_t num_linhas, st_prtcmd_fontAlt *pmt, unsigned char *ref_completa, unsigned char altura_completa);


unsigned char matriz_15[] = {
#include "matrix_16x32_c819.txt"
};

uint16_t libv_ptrcmd_larg_janela, libv_ptrcmd_alt_janela, libv_ptrcmd_y_janela, libv_ptrcmd_x_janela;

uint16_t libv_ptrcmd_alt_char = 24u;
uint16_t libv_ptrcmd_larg_char = 9u;

uint16_t libv_ptrcmd_alt_barcode, libv_ptrcmd_larg_barcode;
uint16_t libv_ptrcmd_alt_grafico, libv_ptrcmd_larg_grafico;

plibv_func_get_mtx_t libv_func_get_mtx_t = mtx_Get9x24;

#define GRAFICO_24_BITS_VERITICAIS  24
#define GRAFICO_8_BITS_VERITICAIS   8

void double_filete(uint8_t * filetein, uint8_t * fileteout, uint8_t nBits);


const unsigned char mask_completa[]= {0x7F,0xFE};

const st_prtcmd_matriz_prop tab_matrizes[] = {
  {
    matriz_15,
    (uint8_t*)mask_completa,
    16,
    32,
    30
  }
};

const st_prtcmd_fontAlt _fontes[] = {
  {                     // PRTCMD_FONTE_9x12
    NULL,               // st_prtcmd_matriz_prop *matriz_ref;
    mtx_Get9x12,        // plibv_func_get_mtx_t func_mtx;
    9,                  // uint8_t larg_char;
    12,                 // uint8_t alt_char;
    1,                  // uint8_t tam_char;
    13,                 // uint8_t alt_mont;
    9,                  // uint8_t larg_mont;
    0,                  // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_9x24
    NULL,               // st_prtcmd_matriz_prop *matriz_ref;
    mtx_Get9x24,        // plibv_func_get_mtx_t func_mtx;
    9,                  // uint8_t larg_char;
    24,                 // uint8_t alt_char;
    3,                  // uint8_t gap_linhas;
    1,                  // uint8_t tam_char;
    27,                 // uint8_t alt_mont;
    9,                  // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    0,                  // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_12x24
    NULL,               // st_prtcmd_matriz_prop *matriz_ref;
    mtx_Get12x24,       // plibv_func_get_mtx_t func_mtx;
    12,                  // uint8_t larg_char;
    24,                 // uint8_t alt_char;
    3,                  // uint8_t gap_linhas;
    1,                  // uint8_t tam_char;
    27,                 // uint8_t alt_mont;
    12,                  // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    0,                  // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_16x32
    NULL,               // st_prtcmd_matriz_prop *matriz_ref;
    mtx_Get16x32,        // plibv_func_get_mtx_t func_mtx;
    16,                  // uint8_t larg_char;
    32,                 // uint8_t alt_char;
    4,                  // uint8_t gap_linhas;
    1,                  // uint8_t tam_char;
    36,                 // uint8_t alt_mont;
    16,                  // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    0,                  // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_24x48
    NULL,               // st_prtcmd_matriz_prop *matriz_ref;
    mtx_Get12x24,       // plibv_func_get_mtx_t func_mtx;
    12,                  // uint8_t larg_char;
    24,                 // uint8_t alt_char;
    3,                  // uint8_t gap_linhas;
    2,                  // uint8_t tam_char;
    27,                 // uint8_t alt_mont;
    12,                  // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    0,                  // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_32x64
    NULL,               // st_prtcmd_matriz_prop *matriz_ref;
    mtx_Get16x32,        // plibv_func_get_mtx_t func_mtx;
    16,                  // uint8_t larg_char;
    32,                 // uint8_t alt_char;
    4,                  // uint8_t gap_linhas;
    2,                  // uint8_t tam_char;
    36,                 // uint8_t alt_mont;
    16,                  // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    0,                  // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_8x12_CIS
    &tab_matrizes[0],   // st_prtcmd_matriz_prop *matriz_ref;
    mtx_get_matCIS,     // plibv_func_get_mtx_t func_mtx;
    8,                  // uint8_t larg_char;
    12,                 // uint8_t alt_char;
    1,                  // uint8_t gap_linhas;
    1,                  // uint8_t tam_char;
    13,                 // uint8_t alt_mont;
    9,                  // uint8_t larg_mont;
    15,                 // uint8_t col_ref;
    31,                 // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_8x23_CIS
    &tab_matrizes[0],   // st_prtcmd_matriz_prop *matriz_ref;
    mtx_get_matCIS,     // plibv_func_get_mtx_t func_mtx;
    8,                  // uint8_t larg_char;
    23,                 // uint8_t alt_char;
    3,                  // uint8_t gap_linhas;
    1,                  // uint8_t tam_char;
    26,                 // uint8_t alt_mont;
    9,                  // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    31,                 // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_10x23_CIS
    &tab_matrizes[0],   // st_prtcmd_matriz_prop *matriz_ref;
    mtx_get_matCIS,     // plibv_func_get_mtx_t func_mtx;
    10,                 // uint8_t larg_char;
    23,                 // uint8_t alt_char;
    3,                  // uint8_t gap_linhas;
    1,                  // uint8_t tam_char;
    26,                 // uint8_t alt_mont;
    11,                 // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    31,                 // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_14x30_CIS
    &tab_matrizes[0],   // st_prtcmd_matriz_prop *matriz_ref;
    mtx_get_matCIS,     // plibv_func_get_mtx_t func_mtx;
    14,                 // uint8_t larg_char;
    30,                 // uint8_t alt_char;
    4,                  // uint8_t gap_linhas;
    1,                  // uint8_t tam_char;
    34,                 // uint8_t alt_mont;
    16,                 // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    0xFF,               // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_20x46_CIS
    &tab_matrizes[0],   // st_prtcmd_matriz_prop *matriz_ref;
    mtx_get_matCIS,     // plibv_func_get_mtx_t func_mtx;
    10,                 // uint8_t larg_char;
    23,                 // uint8_t alt_char;
    3,                  // uint8_t gap_linhas;
    2,                  // uint8_t tam_char;
    26,                 // uint8_t alt_mont;
    11,                 // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    31,                 // uint8_t linha_ref;
  },
  {                     // PRTCMD_FONTE_28x60_CIS
    &tab_matrizes[0],   // st_prtcmd_matriz_prop *matriz_ref;
    mtx_get_matCIS,     // plibv_func_get_mtx_t func_mtx;
    14,                 // uint8_t larg_char;
    30,                 // uint8_t alt_char;
    4,                  // uint8_t gap_linhas;
    2,                  // uint8_t tam_char;
    34,                 // uint8_t alt_mont;
    16,                 // uint8_t larg_mont;
    0,                  // uint8_t col_ref;
    0xFF,               // uint8_t linha_ref;
  },
};

void print_command_task_init( void)
{
  CBuffInit( &CbufStructPrtCmd, ( char*)CbufPrtCmd, PRINT_BUFFER_SIZE);
}

uint16_t bufTempImpressaoAlturas[libx_print_command_TempBufSize];

uint16_t cfg_largura_caractere = 12;
uint16_t cfg_altura_caractere = 24;

//static uint16_t prtCmdNumBytes=0;
//static uint16_t ptParam16_0;
uint8_t prtCmdParams[100];
int dashCount = 0;
int dashCountLine = 0;
uint16_t prtCutterDepth = 0;
uint8_t prtWaitCutterState;

uint8_t StatusResp;


uint16_t libv_prtcmd_numFiletesGrafico = 0;

uint8_t  CaracteresDoCodigoDeBarras[64];
//uint16_t convertTest( uint16_t lin, int width);
uint16_t convertTest( uint16_t lin, int width_in, int width_out);

#define PRT_WAITCUTTER_IDLE     0
#define PRT_WAITCUTTER_FOR_CUT  1
#define PRT_WAITCUTTER_FOR_TEST 2
#define PRT_WAITCUTTER_BUSY     3

void print_command_task( void)
{
  // Aqui vai a task de tratamento dos comandos recebidos para impressão
  // Comandos possíveis (CommandPending[0]:
  //    ESC
  //    GS
  //    0xFF - Impressão de texto
  //    0 - Não tem nada em curso

  if (prtWaitCutterState != PRT_WAITCUTTER_IDLE)                                          //  Se está com corte pendente
  {                                                                                       //  [
    switch (prtWaitCutterState) {                                                         //    Switch (status do corte) [
                                                                                          //      ----------
      case PRT_WAITCUTTER_FOR_CUT:                                                        //      caso está esperando finalizar a impressão para poder cortar
      {                                                                                   //      [
        if (lib_thermalPr_isFillete_Empty()) {                                            //        Se NÃO tem mais filetes para imprimir [
          prtWaitCutterState = PRT_WAITCUTTER_BUSY;                                       //          Coloca o status para aguardar o término do corte
          lib_thermalPr_Cutter_Cut(prtCutterDepth);                                       //          Inicia o corte
        }                                                                                 //        ]
        break;
      }                                                                                   //      ]
                                                                                          //      ----------
      case PRT_WAITCUTTER_FOR_TEST:                                                       //      caso está esperando finalizar a impressão para poder fazer o teste do cutter
      {                                                                                   //      [
        if (lib_thermalPr_isFillete_Empty()) {                                            //        Se NÃO tem mais filetes para imprimir [
          prtWaitCutterState = PRT_WAITCUTTER_BUSY;                                       //          Coloca o status para aguardar o término do teste
          lib_thermalPr_Cutter_Chk();                                                     //          Inicia o teste
        }                                                                                 //        ]
        break;
      }                                                                                   //      ]
                                                                                          //      ----------
      case PRT_WAITCUTTER_BUSY:                                                           //      Caso está aguardando a guilhotina liberar
      {                                                                                   //      [
        if (!libx_thermalPr_IsCutterBusy()) {                                             //        Se a guilhotina estiver liberada [
          prtWaitCutterState = PRT_WAITCUTTER_IDLE;                                       //          Libera o bloqueio da guilhotina
          if( prtCutterDepth == libc_thermalPr_CORTE_TOTAL)                               //          Se corte total
          {                                                                               //          [
            lib_eject_vote_command();                                                     //            ejetar.
          }                                                                               //          ]
          prtCutterDepth = 0;                                                             //          Limpa profundidade de corte
        }                                                                                 //        ]
        break;
      }                                                                                   //      ]
    }                                                                                     //    ]   - Final do switch do cutter
  }                                                                                       //  ]
  else if (libv_prtcmd_numFiletes2Addx) {                                                 //  Senão Se está esperando liberar buffer de filete para poder transferir buffer temporário de impressão [
    if (lib_thermalPr_FreeFilletes_Size() >= libv_prtcmd_numFiletes2Addx) {               //    Se tem espaço no buffer de filetes suficiente [
      int ii;
      for (ii = 0; ii < libv_prtcmd_numFiletes2Addx; ii++) {                              //      Para todos os filetes preparados no buufer temporário de impressão [
        lib_thermalPr_SaveFillete(&bufTempImpressao[ii][0],
            bufTempImpressaoAlturas[ii], TPR_SPEED_NORMAL, LIBC_TPR_COM_IMPRESSAO);       //        Copia o filete para o buffer de filetes
      }                                                                                   //      ]
      lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                                      //      Dispara inicio de impressão
      libv_prtcmd_numFiletes2Addx = 0;                                                    //      Libera o tratamento de comandos
      libv_prtcmd_nlinha = 0;
    }                                                                                     //    ]
  }                                                                                       //  ]
  else if (lib_prtCmd_form_is_printing()) {
    libv_prtcmd_numFiletes2Addx = lib_prtform_print_next_campo(PrtCmdMsg+2);
  }
  else if(libx_prtCmd_RxReady()) {                                                        //  Senão Se tem mensagem de impressão [
//    uint8_t StatusResp;
    uint16_t RespSize;
    StatusResp = 0;
    RespSize = 3;
    libx_prtCmd_getMsgSize(&libv_prtcmd_MsgSize);
    libx_prtCmd_getMessage((char*)PrtCmdMsg, libv_prtcmd_MsgSize);                               //    Pega a mensagem a processar
    switch (PrtCmdMsg[TIPO_COMANDO]) {
      // #######################################################################
      // #########    Comandos de Formulário
      // #######################################################################
      case 'F':
      {
        switch (PrtCmdMsg[SUB_COMANDO])
        {
          // Gerenciamento de formulário
          case 'g':
          {
            switch (PrtCmdMsg[SUB_COMANDO+1])
            {
              // Adição de formulario
              case 'A':
              {
                if (libv_prtcmd_MsgSize == 6) {
                  StatusResp = lib_prtform_add_form(PrtCmdMsg[3],getU16FromU8(PrtCmdMsg,4));
                }
                else {
                  StatusResp = MIV_ERROR_INVALID_SIZE;
                }
                break;
              }
              // Remocao de formulario
              case 'X':
              {
                if (libv_prtcmd_MsgSize == 4) {
                  StatusResp = lib_prtform_delete_form(PrtCmdMsg[3]);
                }
                else {
                  StatusResp = MIV_ERROR_INVALID_SIZE;
                }
                break;
              }
              default:
              {
                StatusResp = MIV_ERROR_INVALID_PARAM;
              }
            }
            break;
          }
          // Gerenciamento de campos de formulário
          case 'c':
          {
            switch (PrtCmdMsg[2])
            {
              // Adiciona Campos a um formulário
              case 'A':
              {
                StatusResp = ( uint8_t)lib_prtform_add_campo(&PrtCmdMsg[3], libv_prtcmd_MsgSize-3);

                break;
              }
              // Exclui campos de um formulário
              case 'X':
              {
                if (libv_prtcmd_MsgSize != 5) {
                  StatusResp = MIV_ERROR_INVALID_SIZE;
                }
                else {
                  StatusResp = ( uint8_t)lib_prtform_delete_campo(&PrtCmdMsg[3]);
                }
                break;
              }
              default:
              {
                StatusResp = MIV_ERROR_INVALID_PARAM;
              }
            }
            break;
          }
          // Impressão de formulários
          case 'p':
          {
            StatusResp = ( uint8_t)lib_prtform_print( ( uint8_t*)&PrtCmdMsg[2], ( uint16_t)libv_prtcmd_MsgSize-2);
            break;
          }
          default:
          {
            StatusResp = MIV_ERROR_COMMAND_INVALID;
          }
        }
        break;
      } // final do case TIPO_COMANDO == 'F'
      // #######################################################################
      // #########    Comandos de impressão bufferizada
      // #######################################################################          //  --------------------------------------------------------
      case 'D':                                                                           //  case Impressão bufferizada
      {                                                                                   //  [
        libt_prtform_form_parse_params_buffered  *params_gen;
        params_gen= (libt_prtform_form_parse_params_buffered *)&PrtCmdMsg[SUB_COMANDO+1];
        if( (PrtCmdMsg[SUB_COMANDO]!='z')  && (PrtCmdMsg[SUB_COMANDO]!='p') ) {           //    Se o comando é comando de montagem [
          uint16_t altura_total, larg_total;
          altura_total = params_gen->altura + params_gen->cordenada_y;                    //      Calcula altura total do elemento com o inicio
          larg_total = params_gen->largura + params_gen->cordenada_x;                     //      Calcula largura total
          if ((altura_total > libx_print_command_TempBufSize) ||                          //      Se a altura ou
              (larg_total > LIBX_THERMALPR_DOTS_POR_FILETE)) {                            //      largura ultrapassam limite [
            StatusResp = MIV_ERROR_INVALID_SIZE;                                              //        Retorna erro de tamanho
            break;                                                                        //        Interrompe o case
          }                                                                               //      ]
        }                                                                                 //    ]

        switch (PrtCmdMsg[SUB_COMANDO]) {                                                 //    switch subcomando de impressão bufferizada [
          case 'z':                                                                       //      case limpeza do buffer
          {                                                                               //      [
            lib_prtcmd_zera_bufTempImpressao();                                           //        Limpa o buffer temporário de impressão
            break;
          }                                                                               //      ]
                                                                                          //      .................................
          case 'p':                                                                       //      case impressao do buffer
          {
            uint16_t TmpSize;
            if ((TmpSize=lib_prtcmd_checkReady( PrtCmdMsg)) > 0) {
              RespSize = TmpSize;
              break;
            }

            libv_prtcmd_numFiletes2Addx  =
                    lib_prtcmd_compacta_bufTempImpressao(getU16FromU8(PrtCmdMsg,2));   //      Compacta o buffer temporário de impressão

            lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                                //  Inicia impressão do buffer temporario de impressão usado
            break;
          }
          case 't':
          {
            libt_prtform_form_parse_params_buffered  *params_gen;
            libt_prtform_form_parse_params_txt  *params_txt;
            uint8_t *pntDados;
            uint16_t tamDados;

            params_gen = (libt_prtform_form_parse_params_buffered *)&PrtCmdMsg[2];
            params_txt = (libt_prtform_form_parse_params_txt*) &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];
            pntDados = &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_TXT_SIZE + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];
            tamDados = *(uint16_t *) pntDados;

            Bool FlagNegrito = ((params_txt->negrito == 'N') || (params_txt->negrito == 'R'));
            Bool FlagReverso = ((params_txt->negrito == 'r') || (params_txt->negrito == 'R'));

            if( (libv_prtcmd_MsgSize <
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_TXT_SIZE +
                        LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 3)) ||
                (libv_prtcmd_MsgSize !=
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_TXT_SIZE +
                        LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + tamDados + 2) ) ){
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            else {
              lib_prtcmd_monta_texto(params_gen->cordenada_y, params_gen->cordenada_x,
                          params_gen->largura, params_gen->altura, FlagReverso,
                          params_txt->fonte , FlagNegrito, (params_txt->alinhamento=='d'),
                          params_txt->wrap, tamDados, pntDados+2);
            }
            break;
          }
          case 'q':
          {
            libt_prtform_form_parse_params_buffered * params_gen;
            libt_prtform_form_parse_params_qr * params_qr;
            uint8_t *pntDados;
            uint16_t tamDados;

            params_gen = (libt_prtform_form_parse_params_buffered *) &PrtCmdMsg[2];
            params_qr = (libt_prtform_form_parse_params_qr*) &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];
            pntDados = &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_QR_SIZE + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];
            tamDados = *(uint16_t *) pntDados;


            lib_prtform_conv_qr_ecc_tipo(&params_qr->qr_ecc, &params_qr->qr_tipo);

            if( ((params_gen->largura < params_qr->qr_largura) &&
                 (params_gen->altura < params_qr->qr_largura)         ) ||
                (libv_prtcmd_MsgSize <
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE +
                          LIBC_PRTFORM_FORM_PARSE_PARAMS_QR_SIZE + 3) ) ||
                (libv_prtcmd_MsgSize !=
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE +
                          LIBC_PRTFORM_FORM_PARSE_PARAMS_QR_SIZE + tamDados + 2) ) ) {
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            else {
              lib_prtcmd_monta_qrcode(params_gen->cordenada_y, params_gen->cordenada_x,
                                     params_qr->qr_largura, params_qr->qr_ecc,
                                     params_qr->qr_tipo, tamDados, pntDados+2);
            }
            break;
          }
          case 'r':
          {
            libt_prtform_form_parse_params_buffered * params_gen;
            libt_prtform_form_parse_params_retangulo * params_retangulo;

            params_gen = (libt_prtform_form_parse_params_buffered *)&PrtCmdMsg[2];
            params_retangulo = (libt_prtform_form_parse_params_retangulo *) &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];

            if( (libv_prtcmd_MsgSize !=
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE +
                          LIBC_PRTFORM_FORM_PARSE_PARAMS_RET_SIZE) ) ) {
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            else {
              lib_prtcmd_monta_retangulo(params_gen->cordenada_y, params_gen->cordenada_x,
                     params_gen->largura, params_gen->altura, params_retangulo->rt_borda);
            }

            break;
          }
          case 'g':
          {
            libt_prtform_form_parse_params_buffered * params_gen;
            libt_prtform_form_parse_params_graf * params_grafico;
            uint8_t *pntDados;
            uint16_t tamDados;

            params_gen = (libt_prtform_form_parse_params_buffered *)&PrtCmdMsg[2];
            params_grafico = (libt_prtform_form_parse_params_graf*) &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];
            pntDados = &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_GRAF_SIZE + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];

            //memset(&bufTempImpressao, 0x00, sizeof(bufTempImpressao));
            tamDados = ((uint16_t) (ceil(((double) params_grafico->graf_largura/8)))) *
                                                             params_grafico->graf_altura;
            if( (libv_prtcmd_MsgSize <
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE +
                          LIBC_PRTFORM_FORM_PARSE_PARAMS_GRAF_SIZE + 3) ) ||
                (libv_prtcmd_MsgSize !=
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE +
                          LIBC_PRTFORM_FORM_PARSE_PARAMS_GRAF_SIZE + tamDados + 2) )||
                (tamDados != * (uint16_t*) pntDados ) ) {
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            else {
              lib_prtcmd_monta_grafico(params_gen->cordenada_y, params_gen->cordenada_x,
                                     params_gen->largura, params_gen->altura,
                                     params_grafico->graf_largura, params_grafico->graf_altura,
                                     tamDados, pntDados+2);
            }

            break;
          }
          case 'b':
          {
            libt_prtform_form_parse_params_buffered * params_gen;
            libt_prtform_form_parse_params_barcode * params_barcode;
            uint8_t *pntDados;
            uint16_t tamDados;

            params_gen = (libt_prtform_form_parse_params_buffered *)&PrtCmdMsg[2];
            params_barcode = (libt_prtform_form_parse_params_barcode *) &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];
            pntDados = &PrtCmdMsg[LIBC_PRTFORM_FORM_PARSE_PARAMS_CB_SIZE + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE + 2];
            tamDados = *(uint16_t *) pntDados;


            if( (libv_prtcmd_MsgSize <
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE +
                          LIBC_PRTFORM_FORM_PARSE_PARAMS_CB_SIZE + 3) ) ||
                (libv_prtcmd_MsgSize !=
                      ( 2 + LIBC_PRTFORM_FORM_PARSE_PARAMS_BUFFERED_SIZE +
                          LIBC_PRTFORM_FORM_PARSE_PARAMS_CB_SIZE + tamDados + 2) ) ) {
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            else {
              if ((params_barcode->cb_tipo == 'I') && (tamDados & 0x01)) {
                memmove(pntDados+3, pntDados+2, tamDados);
                *(pntDados+2) = '0';
                tamDados++;
              }
              *(pntDados+tamDados+2) = 0;
              lib_prtcmd_monta_barcode(params_gen->cordenada_y, params_gen->cordenada_x,
                            params_gen->largura, params_gen->altura,
                            (uint8_t)params_barcode->cb_tipo, params_barcode->cb_modulo,
                            params_barcode->cb_prop3, tamDados, pntDados+2);
            }
            break;
          }
          default:
          {
          StatusResp = MIV_ERROR_COMMAND_INVALID;
          }
        }
        break;
      } // final do case TIPO_COMANDO = 'D'                                               //  ]
      // #######################################################################
      // #########    Comandos de impressão direta
      // #######################################################################          //  --------------------------------------------------------
      case 'd':                                                                           //  case Impressão direta
      {                                                                                   //  [
        uint16_t TmpSize;
        if (((TmpSize = lib_prtcmd_checkReady(PrtCmdMsg)) > 0) &&                         //    Se tem alguma impedimento de impressão e
            (PrtCmdMsg[SUB_COMANDO] != 'z')) {                                            //          não é comando de alterar parâmetro de impressão [
          RespSize = TmpSize;                                                             //      Retorna o erro
          break;                                                                          //
        }                                                                                 //    ]

        switch (PrtCmdMsg[SUB_COMANDO]) {
          // Realiza corte parcial
          case 'c':
          {
            if (prtWaitCutterState == PRT_WAITCUTTER_IDLE) {
              prtWaitCutterState = PRT_WAITCUTTER_FOR_CUT;
              prtCutterDepth = libc_thermalPr_CORTE_PARCIAL;
            }
            else {
              StatusResp = MIV_ERROR_CUTTER_BUSY;
            }
            break;
          }
          // Realiza corte total
          case 'C':
          {
            int printed = libx_get_filete_counter();                            //  numero de filetes ja impressos
            int toprint = libx_get_filete_remain();                             //  numero de filetes a imprimir
            if( ( printed + toprint) < LIMITE_ROLETE)                           //
            {
              StatusResp = MIV_ERROR_CUTTER_NOT_ALLOWED;
            }
            else
            {
              if (prtWaitCutterState == PRT_WAITCUTTER_IDLE) {
                prtWaitCutterState = PRT_WAITCUTTER_FOR_CUT;
                prtCutterDepth = libc_thermalPr_CORTE_TOTAL;
//                lib_eject_vote_command();                                           // após corte, tem que ejetar.
              }
              else {
                StatusResp = MIV_ERROR_CUTTER_BUSY;
              }
            }
            break;
          }
          // Realiza teste de guilhotina
          case 'k':
          {
            if (prtWaitCutterState == PRT_WAITCUTTER_IDLE) {
              lib_thermalPr_cutter_retry( 2);                                   // 2 retries
              prtWaitCutterState = PRT_WAITCUTTER_FOR_TEST;
            }
            else {
              StatusResp = MIV_ERROR_CUTTER_BUSY;
            }
            break;
          }
                                                                                          //      --------------------
          case 'f':                                                                       //      Case comando de avanço de papel
          {                                                                               //      [
            uint16_t alt_avanco;
            alt_avanco = getU16FromU8(PrtCmdMsg,2);                                     //        Pega valor do avanço do papel
            if ( alt_avanco < LIBX_THERMALPR_MAX_ALTURA_FILETE) {                         //        Se o avanço é inferior ao maximo [
              lib_thermalPr_SaveFillete( NULL, alt_avanco,
                                      TPR_SPEED_NORMAL, LIBC_TPR_SEM_IMPRESSAO);          //          Avança o valor solicitado sem impressão
              lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                              //          Aciona o movimento do papel
            }                                                                             //        ]
            else {                                                                        //        Senão [
              StatusResp = MIV_ERROR_INVALID_PARAM;                                           //          Indica qu o parametro é invalido
            }                                                                             //        ]
            break;
          }                                                                               //      ]
                                                                                          //      --------------------
          case 't':                                                                       //      Case impressao direta de texto
          {                                                                               //      [
            uint8_t txt_fonte, txt_negrito, txt_gap_extra, nchars_linha, max_chars;
            uint16_t idx_msg, numFiletes;
            st_prtcmd_fontAlt *param_txt;
            uint8_t rascunho[100];
            uint8_t this_ch, alt_total;

            txt_fonte = PrtCmdMsg[2];                                                     //        Pega o fonte a usar
            txt_negrito = PrtCmdMsg[3];                                                   //        Pega a propriedade do caractere
            txt_gap_extra = PrtCmdMsg[4];                                                 //        Pega valor do gap extra
            PrtCmdMsg[libv_prtcmd_MsgSize] = 0;                                           //        Coloca NULL no final do texto - necessário para terminar

            param_txt = lib_prtcmd_get_par_monta_txt(txt_fonte);                          //        Pega os parâmetros de impressão do fonte selecionado
            alt_total = param_txt->alt_mont*param_txt->tam_char;
            lib_prtcmd_zera_bufTempImpressao();                                           //        Limpa buffer temporário de impressão
            max_chars = LIBX_THERMALPR_DOTS_POR_FILETE /
                        (param_txt->larg_mont*param_txt->tam_char);             //        Calcula número máximo de caracteres por linha
            idx_msg = 5;                                                                  //        Aponta para o primeiro caractere de dados
            nchars_linha = 0;                                                             //        Inicia numero de caracteres no racunho
            numFiletes = 0;                                                               //        Numero de filetes a transferir para o buffer de filetes

            while (idx_msg <= libv_prtcmd_MsgSize) {                                      //        Enquanto não processou todos os caracteres [
              if (nchars_linha == 0) {                                                    //          Se é o primeiro caracteer da linha [
                if (lib_thermalPr_FreeFilletes_Size() < alt_total) {                      //            Se não cabe uma linha na buffer de filetes [
                  StatusResp = MIV_ERROR_MEMORY_FULL;                                     //              Retorna erro de memória full
                  break;                                                                  //              Interrompe a geração de impressão
                }                                                                         //            ]
              }                                                                           //          ]
              this_ch = PrtCmdMsg[idx_msg++];                                             //          pega o dado atual
              if ((this_ch == LF) || (this_ch == 0)) {                                    //          Se chegou ao final da linha ou dos dados [
                if (nchars_linha == 0) {                                                  //            Se não tem dados no rascunho [
                  lib_thermalPr_SaveFillete( NULL, alt_total+txt_gap_extra,
                                          TPR_SPEED_NORMAL, LIBC_TPR_SEM_IMPRESSAO);      //              Pula uma linha
                  lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                          //              Inicia a impress'ao
                }                                                                         //            ]
                else {                                                                    //            Senão [
                  lib_prtcmd_monta_texto(0, 0,LIBX_THERMALPR_DOTS_POR_FILETE,
                              alt_total+txt_gap_extra,
                              ((txt_negrito == 'R')||(txt_negrito == 'r')),
                              txt_fonte , ((txt_negrito == 'R')||(txt_negrito == 'N')),
                              false, 's', nchars_linha, rascunho);                             //              Monta a linha de rascunho
                  numFiletes = lib_prtcmd_compacta_bufTempImpressao(
                                 alt_total+txt_gap_extra);  //              Compacta o buffer temporário de impressão
                }                                                                         //            ]
              }                                                                           //          ]
              else {                                                                      //          Senão [
                rascunho[nchars_linha++] = this_ch;                                       //            Adiciona o caractere no rascunho
                if (nchars_linha == max_chars) {                                          //            Se atingiu final da linha [
                  lib_prtcmd_monta_texto(0, 0,LIBX_THERMALPR_DOTS_POR_FILETE,
                         alt_total+txt_gap_extra,
                         ((txt_negrito == 'R')||(txt_negrito=='r')),
                         txt_fonte , ((txt_negrito == 'R')||(txt_negrito=='N')), false,
                              's', nchars_linha, rascunho);                             //              Monta linha para impressão
                  numFiletes = lib_prtcmd_compacta_bufTempImpressao(
                                                     alt_total+txt_gap_extra);  //              Compacta o buffer temporário de impressão
                }                                                                         //            ]
              }                                                                           //          ]
              if (numFiletes) {                                                           //          Se tem filete a transferir [
                for (int ii = 0; ii < numFiletes; ii++) {                                 //            Para todos os filetes preparados no buufer temporário de impressão [
                  lib_thermalPr_SaveFillete(&bufTempImpressao[ii][0],
                      bufTempImpressaoAlturas[ii], TPR_SPEED_NORMAL,
                              LIBC_TPR_COM_IMPRESSAO);                                    //              Salva o filete no buffer de filetes
                }                                                                         //            ]
                lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                            //            Dispara inicio de impressão
                lib_prtcmd_zera_bufTempImpressao();                                       //            Limpa buffer temporario de impressão
                nchars_linha = 0;                                                         //            "Limpa o rascunho
                numFiletes = 0;                                                           //            Limpa indicador de filetes a transmitir
              }                                                                           //          ]
            }                                                                             //        ]
            break;
          }                                                                               //      ]
                                                                                          //      --------------------
          case 'g':                                                                       //      Case impressão direta de grafico
          {                                                                               //      [
            uint16_t posX, larg_img, alt_img, numBytes;
            posX = getU16FromU8(PrtCmdMsg,2);                                          //        Pega a posição horizontal
            larg_img = getU16FromU8(PrtCmdMsg,4);                                      //        Pega a largura da imagem
            alt_img = getU16FromU8(PrtCmdMsg,6);                                       //        Pega a altura da imagem

            numBytes = (uint16_t) ceil(((double) larg_img) / 8) * alt_img;                //        Calcula o número de bytes
            if ((libv_prtcmd_MsgSize == numBytes + 8) &&                                  //        Se o numero de bytes não condiz com o tamanho da mensagem
                ((posX+larg_img) <= LIBX_THERMALPR_DOTS_POR_FILETE) ){                    //          Ou a imagem vai "estourar na linha" [
              lib_prtcmd_zera_bufTempImpressao();                                         //          Zera buffer temporário de impressão
              lib_prtcmd_monta_grafico(0, posX, larg_img, alt_img,
                                  larg_img, alt_img, numBytes, &PrtCmdMsg[8]);            //          Monta o gráfico
              libv_prtcmd_numFiletes2Addx =
                        lib_prtcmd_compacta_bufTempImpressao(alt_img);                    //          Compacta o buffer temporário de impressão e libera a impressão

              lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                              //          Inicia impressão do buffer temporario de impressão usado
            }                                                                             //        ]
            else {                                                                        //        Senão [
              StatusResp = MIV_ERROR_INVALID_SIZE;                                        //          Dá erro de tamanho invalido
            }                                                                             //        ]
            break;
          }                                                                               //      ]
                                                                                          //      --------------------
          case 'q':
          {
            uint16_t posX, larg_qr;
            uint8_t qr_ecc, qr_modo;
            posX = getU16FromU8(PrtCmdMsg,2);
            larg_qr = getU16FromU8(PrtCmdMsg,4);
            qr_ecc = PrtCmdMsg[6];
            qr_modo = PrtCmdMsg[7];
            if (libv_prtcmd_MsgSize>8) {
              lib_prtcmd_zera_bufTempImpressao();
              lib_prtform_conv_qr_ecc_tipo(&qr_ecc, &qr_modo);
              lib_prtcmd_monta_qrcode(0, posX, larg_qr, qr_ecc, qr_modo,
                  libv_prtcmd_MsgSize-8, &PrtCmdMsg[8]);
              libv_prtcmd_numFiletes2Addx =
                        lib_prtcmd_compacta_bufTempImpressao(larg_qr);                    //      Compacta o buffer temporário de impressão

              lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                                //  Inicia impressão do buffer temporario de impressão usado
            }
            else {
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            break;
          }
                                                                                          //      --------------------
          case 'b':
          {
            uint16_t posX, alt_cb;
            uint8_t cb_mod, cb_prop, cb_tipo;
            int cb_tam;
            posX = getU16FromU8(PrtCmdMsg,2);
            alt_cb = getU16FromU8(PrtCmdMsg,4);
            cb_tipo = PrtCmdMsg[6];
            cb_mod = PrtCmdMsg[7];
            cb_prop = PrtCmdMsg[8];
            cb_tam = libv_prtcmd_MsgSize - 9;
            if ((cb_tam > 0) && (cb_tam <= 40))  {
              if ((cb_tipo == 'I') && (cb_tam & 0x01)) {
                memmove(&PrtCmdMsg[10], &PrtCmdMsg[9], cb_tam);
                PrtCmdMsg[9] = '0';
                cb_tam++;
              }
              PrtCmdMsg[cb_tam+9] = 0;

              lib_prtcmd_zera_bufTempImpressao();
              lib_prtcmd_monta_barcode(0, posX,
                      LIBX_THERMALPR_DOTS_POR_FILETE-posX, alt_cb,
                       cb_tipo, cb_mod, cb_prop, cb_tam, &PrtCmdMsg[9]);
              libv_prtcmd_numFiletes2Addx =
                        lib_prtcmd_compacta_bufTempImpressao(alt_cb);                    //      Compacta o buffer temporário de impressão

              lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                                //  Inicia impressão do buffer temporario de impressão usado
            }
            else {
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            break;
          }
                                                                                          //      --------------------
          case 'r':
          {
            uint16_t posX, alt_rt, larg_rt;
            uint8_t borda_rt;
            posX = getU16FromU8(PrtCmdMsg,2);
            alt_rt = getU16FromU8(PrtCmdMsg,4);
            larg_rt = getU16FromU8(PrtCmdMsg,6);
            borda_rt = PrtCmdMsg[8];
            if ((posX+larg_rt) <= LIBX_THERMALPR_DOTS_POR_FILETE)  {
              lib_prtcmd_zera_bufTempImpressao();
              lib_prtcmd_monta_retangulo(0, posX, larg_rt, alt_rt, borda_rt);
              libv_prtcmd_numFiletes2Addx =
                        lib_prtcmd_compacta_bufTempImpressao(alt_rt);                    //      Compacta o buffer temporário de impressão
              lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);                                //  Inicia impressão do buffer temporario de impressão usado
            }
            else {
              StatusResp = MIV_ERROR_INVALID_SIZE;
            }
            break;
          }
          case 'z':                                                             // fator de queima
          {
//            char szBurnTime[6];
//            int iBurnTime;
//            memset( szBurnTime, 0, 6);
//            memcpy( szBurnTime, ( char*)&PrtCmdMsg[2], 4);
//            sscanf( szBurnTime, "%04d", &iBurnTime);
//            burnTime = ( uint16_t)iBurnTime;
//            if( burnTime < 600) burnTime = 600;
//            if( burnTime > 2000) burnTime = 2000;
            burnParam = PrtCmdMsg[2];
            if( burnParam > '5')  burnParam = '5';
            if( burnParam < '0')  burnParam = '0';
            burnTime = setBurningParam( burnParam);
            break;
          }
          case 'w':                                                             // corte total com acrescimo de LF se necessário.
          {
            int printed = libx_get_filete_counter();                            //  numero de filetes ja impressos
            int toprint = libx_get_filete_remain();                             //  numero de filetes a imprimir
            if( ( printed + toprint) < LIMITE_ROLETE)                           //
            {
              int linefeeds = LIMITE_ROLETE - libx_get_filete_counter() + libx_get_filete_remain() + 1;
              lib_thermalPr_SaveFillete( NULL, linefeeds,
                                      TPR_SPEED_NORMAL, LIBC_TPR_SEM_IMPRESSAO);  // area em branco
              lib_thermalPr_StartPrinting( LIBC_TPR_AVANCO);
            }
            if (prtWaitCutterState == PRT_WAITCUTTER_IDLE)
            {
              prtWaitCutterState = PRT_WAITCUTTER_FOR_CUT;
              prtCutterDepth = libc_thermalPr_CORTE_TOTAL;
//              lib_eject_vote_command();                                         // após corte, tem que ejetar.
            }
            else
            {
              StatusResp = MIV_ERROR_CUTTER_BUSY;
            }
            break;
          }
          case 'y':                                                             // corte total "incondicional!"
          {
            if (prtWaitCutterState == PRT_WAITCUTTER_IDLE)
            {
              prtWaitCutterState = PRT_WAITCUTTER_FOR_CUT;
              prtCutterDepth = libc_thermalPr_CORTE_TOTAL;
//              lib_eject_vote_command();                                           // após corte, tem que ejetar.
            }
            else
            {
              StatusResp = MIV_ERROR_CUTTER_BUSY;
            }
            break;
          }

          default:
          {
            StatusResp = MIV_ERROR_COMMAND_INVALID;
          }
        }
        break;
      } // Final do case TIPO_COMANDO = 'd'                                               //  ]

      default:
      {
        StatusResp = MIV_ERROR_COMMAND_INVALID;
      }
    }
    PrtCmdMsg[2] = StatusResp;
    lib_comm_task_send_block(PrtCmdMsg, RespSize, libv_canal_print_command);

  }


  if( lib_thermalPr_isFillete_Empty() &&
      lib_timeout_is_expired( _libc_timeout_motor_hold) &&
      libx_thermalPr_PaperIsMotorOn()  )
  {
    libx_thermalPr_PaperMotorOff(); //desliga o motor
  }

}

/*  ========================================================================================
     Fnc: st_prtcmd_fontAlt* lib_prtcmd_get_par_monta_txt (uint8_t fonte) ;
     ================================ */
  /**
  @brief Pega paramatros da fonte

  @details Busca ponteiro para uma struct st_prtcmd_fontAlt com os parâmetros para impressão do fonte selecionado

  @param none

  @return none

  @author Ie

  @warning

  */
st_prtcmd_fontAlt* lib_prtcmd_get_par_monta_txt (uint8_t fonte)
{

  st_prtcmd_fontAlt* param_txt;
  switch(fonte)
  {
    case '1':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_9x12];
      break;
    }
    case '2':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_9x24];
      break;
    }
    case '3':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_12x24];
      break;
    }
    case '4':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_16x32];
      break;
    }
    case '6':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_24x48];
      break;
    }
    case '8':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_32x64];
      break;
    }
    case 'A':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_8x12_CIS];
      break;
    }
    case 'B':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_8x23_CIS];
      break;
    }
    case 'C':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_10x23_CIS];
      break;
    }
    case 'D':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_14x30_CIS];
      break;
    }
    case 'E':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_20x46_CIS];
      break;
    }
    case 'F':
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_28x60_CIS];
      break;
    }
    default:
    {
      param_txt = (st_prtcmd_fontAlt*) &tab_fontes[PRTCMD_FONTE_12x24];
    }
  }
  return param_txt;
}


/*  ========================================================================================
     Fnc: char lib_prtcmd_get_matriz(unsigned short posic_char, unsigned char *tab_matriz, unsigned char *matriz_out, unsigned char matriz_size, uint8_t num_cols, uint8_t num_linhas, st_prtcmd_matriz_param *pmt);
     ================================ */
  /**
  @brief Busca as linhas da matriz do caractere

  @details    Esta função retorna as linhas que serão usadas na versão do caractere que será usado.
              Esta função já elimina as linhas e colunas que não serão usadas na matriz derivada.

  @param  unsigned short posic_char     - Posição do caractere que será impresso
  @param  unsigned char *tab_matriz     - Tabela das matrizes
  @param  unsigned char *matriz_out     - Buffer de saída da matriz do caractere - deve ser um array com tamanho suficiente para
                                          retornar o caractere completo
  @param  unsigned char matriz_size     - Tamanho da matriz, para fazer consistência de invasão
  @param  uint8_t num_cols              - Numero de colunas da matriz base
  @param  uint8_t num_linhas            - numero de linhas da matriz base
  @param  st_prtcmd_matriz_param *pmt   - struct com os parâmetros do fonte a ser usado.

  @return -2 - Indica que o buffer de saída não acomoda o caractere

  @author Ie

  @warning

  */
char lib_prtcmd_get_matriz(unsigned short posic_char, unsigned char *tab_matriz, unsigned char *matriz_out, unsigned char matriz_size, uint8_t num_cols, uint8_t num_linhas, st_prtcmd_fontAlt *pmt, unsigned char *ref_completa, unsigned char altura_completa)
{
  unsigned long posic_matriz;
  unsigned char ref_byte, ref_mask;
  char nlinhas;
  unsigned char * pnt_mat;
  int bytes_por_linha;
  uint8_t * mask_byte;
  uint8_t *in_mask,*in_byte, in_bit, *out_byte, out_bit;

  nlinhas = 0;                                                                            //  Inicia contador de linhas do caractere
  bytes_por_linha = (int) ceil((double)num_cols / 8);                                     //  Calcula numero de bytes por filete do caractere
  posic_matriz = posic_char * num_linhas * bytes_por_linha;                               //  Localiza o caractere na matriz completa
  pnt_mat = tab_matriz + posic_matriz;                                                    //  Aponta para para o inicio do caractere
  ref_byte = pmt->col_ref / 8;                                                            //  Localiza ofset da coluna de controle de linhas
  ref_mask = 0x80 >> (pmt->col_ref % 8);                                                  //  Monta a mascara para consistir a colunha de controle das linhas
  if (pmt->linha_ref == 0xFF) {                                                           //  Se é o caractere completo [
    mask_byte = ref_completa;
    memcpy(matriz_out, pnt_mat, altura_completa*bytes_por_linha);                         //    Copias as linhas de dados "úteis" do caractere
    nlinhas = altura_completa;                                                            //    Atualiza altura do caractere
  }                                                                                       //  ]
  else {                                                                                  //  Senão [
    mask_byte = pnt_mat+(pmt->linha_ref*bytes_por_linha);
    out_byte = matriz_out;
    for (int ii=0; ii < num_linhas; ii++) {                                               //    Para todas as linhas do caracrere [
      if (*(pnt_mat+ref_byte) & ref_mask) {                                               //      Se a linha está marcada para ser usada [
        nlinhas++;                                                                        //        Incrementa o numero de linhas do caractere
        if (nlinhas >= matriz_size) {                                                     //        Se vai estourar o buffer de saída [
          return -2;                                                                      //          Retorna erro
        }                                                                                 //        ]
        memcpy(out_byte,pnt_mat, bytes_por_linha);                                        //        Copia os bytes da linha para a saída
        out_byte += bytes_por_linha;                                                      //        Pula para a proxima posição de saída
      }                                                                                   //      ]
      pnt_mat += bytes_por_linha;                                                         //      Pula para a proxima posição de entrada
    }                                                                                     //    ]
  }                                                                                       //  ]

  for(int ii=0; ii < nlinhas; ii++) {                                                     //  Para todas as linhas do caractere [
    out_byte = matriz_out + (ii*bytes_por_linha);                                         //    Aponta para o primeiro byte de sapida
    out_bit = 0x80;                                                                       //    Máscara da montagem do bit ativo
    in_byte = matriz_out + (ii*bytes_por_linha);                                          //    Aponta para o primeiro byte da matriz completa
    in_mask = mask_byte;                                                                  //    Aponta para o primeiro byte da referencia
    in_bit = 0x80;                                                                        //    Mascara para varrer a referencia e avaliar se o bit da matriz completa é usado
    for(int jj=0; jj < num_cols; jj++) {                                                  //    Para todas as colunas [
      if(*in_mask & in_bit) {                                                             //      Se a coluna é usada na matriz gerada [
        if (*in_byte & in_bit) {                                                          //        Se o bit da matriz está ativo [
          *out_byte |= out_bit;                                                           //          Seta o bit de saída
        }                                                                                 //        ]
        else {                                                                            //        Senão [
          *out_byte &= ~out_bit;                                                          //          Limpa o bit de saída
        }                                                                                 //        ]

        out_bit >>= 1;                                                                    //        Avança o bit de saida
        if (out_bit == 0) {                                                               //        Se já salvou os 8 bits [
          out_byte++;                                                                     //          Avança para o proximo byte
          out_bit = 0x80;                                                                 //          Reinicia mascara
        }                                                                                 //        ]
      }                                                                                   //      ]
      *in_byte &= ~in_bit;                                                                //      Limpa o bit analisado, pois é o mesmo buffer da saída
      in_bit >>= 1;                                                                       //      Acança o bit de varredura
      if (in_bit == 0) {                                                                  //      Se já analisou os 8 bits [
        in_mask++;                                                                        //        Pula o byte da mascara
        in_byte++;                                                                        //        Pula o byte da matriz
        in_bit = 0x80;                                                                    //        Reinicia mascara
      }                                                                                   //      ]
    }                                                                                     //    ]
  }                                                                                       //  ]
  return nlinhas;                                                                         //  Retorna numero de linhas do caractere
}


/*  ========================================================================================
     Fnc: uint8_t lib_prtcmd_monta_texto(uint16_t linha_bti, uint16_t coluna_bti, uint16_t larg_janela, uint16_t altura_janela, Bool reverso, uint8_t fonte, Bool negrito, Bool Alinha_direita, uint8_t wrap, uint16_t txt_size, uint8_t * txt2print) {;
     ================================ */
  /**
  @brief Monta um texto dentro da janela de impressão

  @details     Esta função monta uma impressão de texto dentro da janela definida por linha_bti, coluna_bti, larg_janela e altura_janela

  @param  uint16_t linha_bti      - Linha do BTI do canto esquerdo superior da janela de impressão do texto
  @param  uint16_t coluna_bti     - Coluna do BTI do canto esquerdo superior da janela de impressão do texto
  @param  uint16_t larg_janela    - Largura da janela de impressão do texto
  @param  uint16_t altura_janela  - Altura da janela de impressão do texto
  @param  Bool reverso            - Indica se o texto deve se gerado em modo reverso
  @param  uint8_t fonte           - Código da fonte a ser usada
  @param  Bool negrito            - Indica se o texto deve ser gerado com negrito
  @param  Bool Alinha_direita     - Indica se o texto deve ser alinhado à direita
  @param  uint8_t wrap            - Indica
  @param  uint16_t txt_size
  @param  uint8_t * txt2print

  @return none

  @author Ie

  @warning

  Coisas que podem ser melhoradas:
    - O lib_prtcmd_add_dots2both poderia ter um parâmetro de repetição pegasse o padrão de entrada e repetisse N vezes.
      Isto evitaria de ter um loop fora e outro dentro.
    - De qualquer forma, como não sabemos qual o tamanho que pode entrar, teria que ter um refresh do WWDG.

  */
uint8_t lib_prtcmd_monta_texto(uint16_t linha_bti, uint16_t coluna_bti, uint16_t larg_janela, uint16_t altura_janela, Bool reverso, uint8_t fonte, Bool negrito, Bool Alinha_direita, uint8_t wrap, uint16_t txt_size, uint8_t * txt2print) {
  uint16_t qtde_caractere_horizontal = 0;
  uint8_t alt_mont = 0;
  uint8_t larg_mont = 0;
  uint8_t gap_linhas;
  uint8_t tam_char, larg_char, alt_char;
  uint16_t linhas_usadas=0;
  uint8_t tmp_cont;
  uint16_t offset_linha;
  uint8_t tabout[35][2];
  uint8_t marg_char;
  uint16_t linhavazia = 0;
  uint16_t  linhacheia = 0xFFFF;
  uint8_t pos_bit2burn;

//  param_monta_txt *param_txt;
  st_prtcmd_fontAlt *param_txt;

  uint8_t bTmp[4];
//  uint8_t filete_out[20];
  uint8_t this_char;
  uint8_t posic_char;

  if (txt_size == 0) {                                                                    //  Se não tem texto para imprimir [
    return 0;                                                                             //    Retorna
  }                                                                                       //  ]

  param_txt = lib_prtcmd_get_par_monta_txt(fonte);                                        //  Pega os parâmetros da fonte usada

  libv_func_get_mtx_t = param_txt->func_mtx;
  larg_char = param_txt->larg_char;
  alt_char = param_txt->alt_char;
  tam_char = param_txt->tam_char;
  gap_linhas = param_txt->gap_linhas;
  alt_mont = param_txt->alt_mont;
  larg_mont = param_txt->larg_mont;

  if(Alinha_direita)                                                                      //  Se está alinhando à direita [
  {
    uint8_t offset_alinhamento_direita = (larg_janela -
                  ((larg_janela / (larg_mont*tam_char)) * (larg_mont*tam_char)));         //    Calcula "desalinhamento" à esquerda para numero total de caracteres possíveis
    coluna_bti += offset_alinhamento_direita;                                             //    Ajusta o início da janema para ter numero inteiro de caracteres
    larg_janela -= offset_alinhamento_direita;                                            //    Ajusta a largura da janela
  }                                                                                       //  ]

  qtde_caractere_horizontal = larg_janela / (larg_mont*tam_char);

  tmp_cont = 0;                                                                           //  Zera contador de caracteres na linha
  while(txt_size-- > 0) {                                                                 //  Enquanto tem caracteres no buffer [
    this_char = txt2print[tmp_cont++];                                                    //    Pega o byte a imprimir
    if( (txt_size == 0) ||                                                                //    Se é fim do buffer ou
        (this_char == LF) ||                                                              //          é LF ou
        (tmp_cont == qtde_caractere_horizontal)) {                                        //          chegou ao final da janela [
      if (this_char == LF) {                                                              //      Se é LF [
        tmp_cont--;                                                                       //        Desconta o LF
      }                                                                                   //      ]

      if (Alinha_direita) {                                                               //      Se é para alinhar à direita [
        offset_linha = larg_janela - (tmp_cont*larg_mont*tam_char);                                //        Acha a posição inicial para que o texto fique alinhado à direita
      }                                                                                   //      ]
      else {                                                                              //      Senão [
        offset_linha = 0;                                                                 //        Começa no inicio da janela
      }                                                                                   //      ]

      for (int ii=0; ii < tmp_cont; ii++) {                                               //      Para todos os caracteres da linha [

        if (param_txt->matriz_ref) {                                                      //        Se é com a matrix alternativa [
          posic_char = ajusta_posic_matriz(txt2print[ii]);                                //          Ajusta a posição do caractere
          lib_prtcmd_get_matriz(posic_char, (uint8_t*)param_txt->matriz_ref->tab_address,
                                &tabout[0][0], 35, 16, 32, param_txt,
                                (uint8_t*)mask_completa,30);                              //          Busca as informações da matriz
        }                                                                                 //        ]

        for (int jj = 0; jj < alt_char; jj++) {                                           //        Para todos os filetes do caractere [

          memset(bTmp,0,sizeof(bTmp));                                                    //          Prepara o buffer para pegar o filete do caractere
          libv_func_get_mtx_t(&tabout[0][0], param_txt,
                                  txt2print[ii], jj, &bTmp[0], negrito);                  //          Pega o filete atual

          for(int kk=0; kk < tam_char; kk++) {                                            //          Para os filetes que precisa queimar repetido [
            uint8_t scanner_bit, scanner_byte;
            pos_bit2burn = 0;                                                             //            Começa do inicio do caractere
            marg_char = (larg_mont-larg_char)/2;                                          //          Calcula a margem esquerda
            for (int mm=0; mm < marg_char; mm++) {                                        //            Para todos os pontos antes do caracteres [
              lib_prtcmd_add_dots2both( bufTempImpressao[linha_bti+(jj*tam_char)+kk],
                        (coluna_bti+offset_linha+pos_bit2burn), (uint8_t *)&linhavazia, 0,
                        tam_char, !reverso);                                              //              Coloca uma sequencia de tam_char pontos vazios
              pos_bit2burn += tam_char;                                                   //              Avança numero de pontos adicionados
            }                                                                             //            ]

            scanner_bit = 0x80;                                                           //            Inicia variaveis para varrer o caractere
            scanner_byte = 0;
            for (int mm=param_txt->larg_char + (negrito ? 1 : 0); mm > 0; mm--) {         //            Para todos os pontos antes do caracteres [
              if (bTmp[scanner_byte] & scanner_bit) {                                     //              Se o bit é ativo [
                lib_prtcmd_add_dots2both( bufTempImpressao[linha_bti+(jj*tam_char)+kk],
                      (coluna_bti+offset_linha+pos_bit2burn), (uint8_t *)&linhacheia, 0,
                      tam_char, !reverso);                                                //                Coloca uma sequencia de tam_char pontos ativos
              }                                                                           //              ]
              else {                                                                      //              Senão [
                lib_prtcmd_add_dots2both( bufTempImpressao[linha_bti+(jj*tam_char)+kk],
                      (coluna_bti+offset_linha+pos_bit2burn), (uint8_t *)&linhavazia, 0,
                      tam_char, !reverso);                                                //                Coloca uma sequencia de tam_char pontos vazios
              }                                                                           //              ]
              pos_bit2burn += tam_char;                                                   //              Avança pontos adicionados
              scanner_bit >>= 1;                                                          //              Avança bit analisado
              if (!scanner_bit) {                                                         //              Se já analisou todo o byte atual [
                scanner_bit = 0x80;                                                       //                Reinicia a mascara de avaliação
                scanner_byte++;                                                           //                Pula para o proximo byte
              }                                                                           //              ]
            }                                                                             //            ]

            marg_char = larg_mont - larg_char - marg_char;                                //            Calcula numero de pontos vazios a adicionar no final
            for (int mm=0; mm < marg_char; mm++) {                                        //            Para todos os pontos depois do caracteres [
              lib_prtcmd_add_dots2both( bufTempImpressao[linha_bti+(jj*tam_char)+kk],
                      (coluna_bti+offset_linha+pos_bit2burn), (uint8_t *) &linhavazia, 0,
                      tam_char, !reverso);                                                //              Coloca uma sequencia de tam_char pontos vazios
              pos_bit2burn += tam_char;                                                   //              Avança numero de pontos adicionados
            }                                                                             //            ]
          }                                                                               //          ]
        }                                                                                 //        ]
        offset_linha += larg_mont*tam_char;                                               //        Pula para o proximo caractere da linha
      }                                                                                   //      ]

      txt2print += tmp_cont;                                                              //      Avança ponteiro de varredura do texto a imprimir
      if (this_char == LF) {                                                              //      Se o caractere que disparou a impressão foi LF
        txt2print++;                                                                      //        Pula o LF
      }                                                                                   //      ]
      tmp_cont = 0;                                                                       //      Zera contador de caracteres da linha
      linha_bti += alt_mont*tam_char+gap_linhas;                                          //      Avança para a proxima linha do buffer temporario de impressão
      linhas_usadas += alt_mont*tam_char + gap_linhas;                                    //      Adiciona numero de linhas usadas
      if (altura_janela - linhas_usadas < alt_mont*tam_char) {                            //      Se a altura da janela não comporta mais uma linha ou
        break;                                                                            //        Interrompe a impressão
      }                                                                                   //      ]
    }                                                                                     //    ]
  }                                                                                       //  ]
  return linhas_usadas;                                                                   //  Retorna numero de linhas usadas
}

uint16_t lib_prtcmd_monta_retangulo(uint16_t linha_bti, uint16_t coluna_bti, uint16_t largura_janela, uint16_t altura_janela, uint8_t  borda)
{

  uint8_t pitch[48];

  if ((largura_janela == 0) || (altura_janela == 0)) {
    return 0;
  }

  memset(&pitch[0], 0xFF, 48);

  if (borda == 0) {
    borda = (altura_janela/2) + 1;
  }

  for(uint16_t ii = 0; ii < borda; ii++) // barra superior
  {
    lib_prtcmd_add_dots2burn( bufTempImpressao[linha_bti+ii],
                             (coluna_bti), pitch, 0, largura_janela);
  }

  if (borda < (altura_janela/2)) {
    for(uint16_t ii = borda; ii < (altura_janela - borda); ii++) // linha vertical esquerda e direita
    {
      lib_prtcmd_add_dots2burn( bufTempImpressao[linha_bti+ii], // esquerda
                                (coluna_bti), pitch, 0, borda);

      lib_prtcmd_add_dots2burn( bufTempImpressao[linha_bti+ii], // direita
                                (coluna_bti+(largura_janela - borda)), pitch, 0, borda);
    }
  }

  for(uint16_t ii = (altura_janela - borda); ii < altura_janela; ii++) // barra inferior
  {
       lib_prtcmd_add_dots2burn( bufTempImpressao[linha_bti+ii],
                                 (coluna_bti), pitch, 0, largura_janela);
  }

  return linha_bti+altura_janela;
}



/*  ========================================================================================
     Fnc: uint16_t lib_prtcmd_monta_grafico(uint16_t linha_bti, uint16_t coluna_bti, uint16_t largura_janela, uint16_t altura_janela, uint16_t largura_grafico, uint16_t altura_grafico, uint16_t size_grafico, uint8_t *dados) {;
     ================================ */
  /**
  @brief Monta um gráfico no buffer temporário de impressão

  @details

  @param none

  @return none

  @author Ie

  @warning

  */
uint16_t lib_prtcmd_monta_grafico(uint16_t linha_bti, uint16_t coluna_bti, uint16_t largura_janela, uint16_t altura_janela, uint16_t largura_grafico, uint16_t altura_grafico, uint16_t size_grafico, uint8_t *dados) {

  if (size_grafico == 0) {
    return 0;
  }

  uint16_t largura_bytes = ceil((double)largura_grafico/8);
  if (largura_grafico > largura_janela) {
    largura_grafico = largura_janela;
  }
  if (altura_grafico > altura_janela) {
    altura_grafico = altura_janela;
  }
  if (largura_grafico > 0) {
    for(uint16_t ii = 0; ii < altura_grafico; ii++)
    {
      lib_prtcmd_add_dots2burn( bufTempImpressao[ii + linha_bti],
                                  coluna_bti, dados, 0, largura_grafico);
      dados += largura_bytes;
    }
    return altura_grafico;
  }
  else {
    return 0;
  }
}

int16_t lib_prtcmd_monta_barcode(uint16_t linha_bti, uint16_t coluna_bti, uint16_t largura_janela, uint16_t altura_janela, uint8_t cb_tipo, uint8_t cb_modulo, uint8_t cb_prop, uint8_t cb_tam, uint8_t *cb_dados)
{
  size_t bitsWritten = 0;

  if (cb_tam == 0) {
    return 0;
  }
  bitsWritten = (size_t) bc_Encode(cb_modulo, cb_prop, cb_tipo, (char*) cb_dados, filletBuffer);

  if (bitsWritten == 0) {
    return MIV_ERROR_INVALID_PARAM;
  }

  if (largura_janela < bitsWritten) {
    bitsWritten = largura_janela;
  }

  for(uint16_t ii = 0; ii < altura_janela; ii++)
  {
    lib_prtcmd_add_dots2burn( bufTempImpressao[linha_bti+ii],
                                        coluna_bti, filletBuffer, 0, bitsWritten);
  }

  return altura_janela;
}

/*  ========================================================================================
     Fnc: uint16_t lib_prtcmd_monta_qrcode(uint16_t linha_bti, uint16_t coluna_bti, uint16_t larg_janela, uint16_t altura_janela, uint16_t largura_qr, uint8_t qr_ecc, uint8_t qr_tipo, uint16_t qr_tam, uint8_t *qr_dados);
     ================================ */
  /**
  @brief monta um código QR dentro de uma janela

  @details

  @param uint16_t linha_bti       Linha do buffer temporário de impressão onde começa a janela do QR
  @param  uint16_t coluna_bti     Coluna do buffer temporário de impressão onde começa a janela do QR
  @param  uint16_t largura_qr     Largura do QR code
  @param  uint8_t qr_ecc          Código de correção de erro do QRcode
                                  LIBC_PRTFORM_QR_ECC_L, LIBC_PRTFORM_QR_ECC_M, LIBC_PRTFORM_QR_ECC_Q, LIBC_PRTFORM_QR_ECC_H
  @param  uint8_t qr_tipo         Código do tipo de codificação dos dados do QRcode
                                  LIBC_PRTFORM_QR_TIPO_NUM, LIBC_PRTFORM_QR_TIPO_ALFANUM, LIBC_PRTFORM_QR_TIPO_8BITS
  @param  uint16_t qr_tam         Número de bytes do QRcode
  @param  uint8_t *qr_dados       Ponteiro para o início dos dados do QRcode

  @return none

  @author Ie

  @warning

  */
uint16_t lib_prtcmd_monta_qrcode(uint16_t linha_bti, uint16_t coluna_bti, uint16_t largura_qr, uint8_t qr_ecc, uint8_t qr_tipo, uint16_t qr_tam, uint8_t *qr_dados)
{
  uint8_t bufBitsQR[17*133];      // Cabe até versão 29 133x133
  int qrCodeSize;

  if (qr_tam == 0) {
    return 0;
  }

  // convertendo código do ECC entre o código do print_formulario para libqrcode
  switch (qr_ecc) {
    case LIBC_PRTFORM_QR_ECC_M:
    {
      qr_ecc = QR_ECLEVEL_M;
      break;
    }
    case LIBC_PRTFORM_QR_ECC_Q:
    {
      qr_ecc = QR_ECLEVEL_Q;
      break;
    }
    case LIBC_PRTFORM_QR_ECC_H:
    {
      qr_ecc = QR_ECLEVEL_H;
      break;
    }
    default:
    {
      qr_ecc = QR_ECLEVEL_L;
      break;
    }
  }

  // convertendo código do QR mode entre o código do print_formulario para libqrcode
  switch (qr_tipo) {
    case LIBC_PRTFORM_QR_TIPO_NUM:
    {
      qr_tipo = QR_MODE_NUM;
      break;
    }
    case LIBC_PRTFORM_QR_TIPO_ALFANUM:
    {
      qr_tipo = QR_MODE_AN;
      break;
    }
    default:
    {
      qr_tipo = QR_MODE_8;
      break;
    }
  }

  qr_dados[qr_tam] = 0;
  qrCodeSize = QRcode_inBits(qr_tam, qr_dados, qr_tipo, qr_ecc, sizeof(bufBitsQR), bufBitsQR);
  if (qrCodeSize == 0) {
    return LIBC_PRTFORM_ER_PARAM_MEMORIA;
  }
  if (qrCodeSize == -1) {
    return LIBC_PRTFORM_ER_PARAM_TAMANHO;
  }

  if (qrCodeSize > largura_qr) {
    return LIBC_PRTFORM_ER_PARAM_TAMANHO;
  }

  int ii, jj;
  uint8_t maskBits;


  uint8_t bits16x1[2] = {0xFF,0xFF};
  uint8_t *ptData;
  uint16_t alturaPronta, larguraPronta, larguraCol, alturaLinha;
  alturaLinha = larguraCol = largura_qr;

  ptData = bufBitsQR;
  alturaPronta = larguraPronta = 0;

  for (ii=0; ii < qrCodeSize; ii++) {
    maskBits = 0x80;
    larguraPronta = 0;
    for (jj=0; jj < qrCodeSize; jj++) {
      if (largura_qr > 16) {
        larguraCol = ((jj+1) * largura_qr / qrCodeSize) - larguraPronta;
        if (larguraCol > 16) {
          larguraCol = 16;
        }
      }
      if (*ptData & maskBits) {
        lib_prtcmd_add_dots2burn( bufTempImpressao[linha_bti+alturaPronta],
                                  coluna_bti+larguraPronta, bits16x1, 0, larguraCol);
      }
      larguraPronta += larguraCol;
      maskBits = maskBits >> 1;
      if (maskBits == 0) {
        maskBits = 0x80;
        ptData++;
      }
    }
    if (largura_qr > 16) {
      alturaLinha = ((ii+1) * largura_qr/ qrCodeSize) - alturaPronta;
      if (alturaLinha > 16) {
        alturaLinha = 16;
      }
    }
    for (jj=1; jj < alturaLinha; jj++) {
      lib_prtcmd_add_dots2burn( bufTempImpressao[linha_bti+alturaPronta+jj],coluna_bti,
                 bufTempImpressao[linha_bti+alturaPronta], coluna_bti, larguraPronta);
    }
    alturaPronta += alturaLinha;
    if (maskBits != 0x80) {
      ptData++;
    }
  }
  return alturaPronta;
}

void lib_prtcmd_zera_bufTempImpressao(void)
{
  for (int ii = 0;ii < libx_print_command_TempBufSize; ii++) {                                //  inicia as alturas de todos os filetes com 1
    bufTempImpressaoAlturas[ii] = 1;
  }

  memset(&bufTempImpressao, 0x00, sizeof(bufTempImpressao));                              //  Limpa o buffer de pontos
}

unsigned short lib_prtcmd_compacta_bufTempImpressao(unsigned short nFiletes)
{
  int idx_ref, idx_obj;

  // Compara linhas consecutivas e contabiliza total, zerando a repetida
  for (idx_ref = 0; idx_ref < nFiletes-1;) {                                              //  Para todos os filetes [
    for (idx_obj = idx_ref+1; idx_obj < nFiletes; idx_obj++) {                            //    Para todos os filetes seguintes [
      if((memcmp(bufTempImpressao[idx_ref], bufTempImpressao[idx_obj],
                                      LIBX_THERMALPR_BYTES_POR_FILETE) == 0) &&           //      Se o filete é igual
          (idx_ref < LIBX_THERMALPR_MAX_ALTURA_FILETE) ) {                                //          e ainda não atingiu o máximo [
          bufTempImpressaoAlturas[idx_ref] += bufTempImpressaoAlturas[idx_obj];           //        Incrementa contador de repetição
          bufTempImpressaoAlturas[idx_obj] = 0;                                           //        Zera contador da linha repetida
      }                                                                                   //      ]
      else {                                                                              //      Senão [
        break;                                                                            //        Interrompe a sequencia
      }                                                                                   //      ]
    }                                                                                     //    ]
    idx_ref = idx_obj;                                                                    //    Começa nova sequencia onde deu diferença
  }                                                                                       //  ]

  //Pula linhas não repetidas
  for (idx_ref = 1; idx_ref < nFiletes; idx_ref++) {                                      //  Para todos os filetes, pulando o primeiro [
    if(bufTempImpressaoAlturas[idx_ref] == 0) {                                           //    Se não é um repetido anulado [
      break;                                                                              //      Interrompe busca
    }                                                                                     //    ]
  }                                                                                       //  ]

  for (idx_obj = idx_ref+1; idx_obj < nFiletes; idx_obj++) {                              //  Para todos os filetes  a partir do primeiro anulado [
    if(bufTempImpressaoAlturas[idx_obj] != 0) {                                           //    Se a linha não está anulada
      memcpy(&bufTempImpressao[idx_ref], &bufTempImpressao[idx_obj],
                                          LIBX_THERMALPR_BYTES_POR_FILETE);           //      Copia a linha para a posição vaga
      bufTempImpressaoAlturas[idx_ref] = bufTempImpressaoAlturas[idx_obj];                //      Copia a altura do filete
      bufTempImpressaoAlturas[idx_obj] = 0;
      idx_ref++;                                                                          //      Avança para a proxima posição de gravar
    }                                                                                     //    ]
  }                                                                                       //  ]
  return idx_ref;                                                                         //  Retorna o novo numero de filetes
}


// Rotina para processar dots de um array para queimar. transfere os bits 1 para o array de saída
void lib_prtcmd_add_dots2burn(uint8_t *enderecoOut, uint16_t posicaoOut, uint8_t *enderecoIn, uint8_t posicaoIn,uint16_t nBit)
{
  lib_prtcmd_add_dots2both(enderecoOut, posicaoOut, enderecoIn, posicaoIn,nBit, true);
}


// Rotina para processar impressão reversa.
void lib_prtcmd_add_dots2clear(uint8_t *enderecoOut, uint16_t posicaoOut, uint8_t *enderecoIn, uint8_t posicaoIn,uint16_t nBit)
{
  lib_prtcmd_add_dots2both(enderecoOut, posicaoOut, enderecoIn, posicaoIn,nBit, false);
}
void lib_prtcmd_add_dots2both(uint8_t *enderecoOut, uint16_t posicaoOut, uint8_t *enderecoIn, uint8_t posicaoIn,uint16_t nBit, bool FlagBurn)
{
  uint8_t *ptrIn;
  uint8_t mascaraIn;
  uint8_t *ptrOut;
  uint8_t mascaraOut;
  int i;

  ptrOut = enderecoOut + (posicaoOut / 8);         // Acha o byte onde salvar os bit
  mascaraOut = 1 << (7 - (posicaoOut % 8));         // Mascara do bit que deve ser salvo
  ptrIn = enderecoIn + (posicaoIn / 8);
  mascaraIn = 1 << (7 - (posicaoIn % 8));

  for (i = 0; i < nBit; i++) {				//  Para todos os bits a processar
    if(*ptrIn & mascaraIn)					//    se o bit está setado
    {										//    [
      if (FlagBurn) {						//      Se é para quermar [
        *ptrOut |= mascaraOut;				//        Seta o bit na saida
      }										//      ]
      else {								//      Senao [
        *ptrOut &= ~mascaraOut;				//        Limpa o bit na saida
      }
    }
    else {					//
      if (!FlagBurn) {			//Se é para limpar a queima
        *ptrOut |= mascaraOut;	//  Limpa o bit da queiam
      }
    }
    mascaraIn >>= 1;
    if (mascaraIn == 0) {
      mascaraIn = 0x80;
      ptrIn++;
    }
    mascaraOut >>= 1;
    if (mascaraOut == 0) {
      mascaraOut = 0x80;
      ptrOut++;
    }
  }
}

void mtx_get_matCIS(uint8_t *matrizChar, st_prtcmd_fontAlt *strFont, char character, uint8_t filletNum, uint8_t *bTmp, Bool negrito)
{
  uint8_t nbytesNormal, nbytesBold;
//  union union_8_16_32 {
//    uint8_t _8[4];
//    uint16_t _16[2];
//    uint32_t _32;
//  } un_8_16_32;

  nbytesNormal = ceil(((double) strFont->matriz_ref->num_cols)/8);
  nbytesBold = ceil(((double) strFont->matriz_ref->num_cols+1)/8);
  memset(bTmp,0,nbytesBold);
  memcpy(bTmp, matrizChar+(nbytesNormal*filletNum), nbytesNormal);
  if (negrito) {
    uint8_t tmp8_a,tmp8_b;
    tmp8_b = 0;
    for (int ii=0; ii < nbytesBold; ii++) {
      tmp8_a = (bTmp[ii] & 0x01) ? 0x80 : 0;
      bTmp[ii] = bTmp[ii] | (bTmp[ii] >> 1) | tmp8_b;
      tmp8_b = tmp8_a;
    }
  }
}

uint8_t ajusta_posic_matriz(uint8_t valor_char)
{
  if (valor_char >= 0xA1) {
    return valor_char - 0x20 - (0xA1-0x7F);
  }
  else if ((valor_char > 0x1F) && (valor_char < 0x7F)) {
    return valor_char - 0x20;
  }
  else {
    return 0;
  }
}
/************************************************************************************************************/
void mtx_Get12x24(uint8_t *matrizChar, st_prtcmd_fontAlt *strFont, char character, uint8_t filletNum, uint8_t *bTmp, Bool negrito)
/************************************************************************************************************/
{
   uint16_t conteudoFilete = 0;

   uint8_t caractereNaoSinalizado = (uint8_t) character;

   if (!(((caractereNaoSinalizado >= 0) && (caractereNaoSinalizado <= 32u))
       || ((caractereNaoSinalizado >=  127u) && (caractereNaoSinalizado <= 160u))))
   { // É um caractere com conteúdo definido?
      if (filletNum < 24) // É um filete válido?
      {
         uint16_t desloc = ((uint16_t)(caractereNaoSinalizado - ((caractereNaoSinalizado > 160u)
                  ? (33u + 34u) : 33u))) * 24u;

         conteudoFilete = matrix12x24Cp819[desloc + ((uint16_t)filletNum)];

         if(negrito)
         {
           conteudoFilete = ((conteudoFilete >> 1) | conteudoFilete);
         }

         bTmp[0] = (conteudoFilete >> 8) & 0x00FF;
         bTmp[1] = (conteudoFilete) & 0x00FF;
      }
   }
   else {
     bTmp[0] = 0;
     bTmp[1] = 0;
   }
}

/************************************************************************************************************/

/************************************************************************************************************/
void mtx_Get9x12(uint8_t *matrizChar, st_prtcmd_fontAlt *strFont, char character, uint8_t filletNum, uint8_t *bTmp, Bool negrito)
/************************************************************************************************************/
{
   uint16_t conteudoFilete = 0;
   uint8_t caractereNaoSinalizado = (uint8_t) character;

   if (!(((caractereNaoSinalizado >= 0) && (caractereNaoSinalizado <= 32u))
       || ((caractereNaoSinalizado >=  127u) && (caractereNaoSinalizado <= 160u))))
   { // É um caractere com conteúdo definido?
      if (filletNum < 12u) // É um filete válido?
      {
        filletNum = (filletNum * 24u) / 12u; // Converte de 32 para 24 para usar a fonte de 24 de altura.
         uint16_t desloc = ((uint16_t)(caractereNaoSinalizado - ((caractereNaoSinalizado > 160u)
                  ? (33u + 34u) : 33u))) * 24u;

         /*
          * Ocupa 2 bytes, incluindo 1 bit em branco à esquerda do desenho (MSb - bit mais significativo).
          */
         conteudoFilete = (((uint16_t) matrix8x24Cp819[desloc + ((uint16_t)filletNum)]) << 7) & 0x7F80u;

         if(negrito)
         {
           conteudoFilete = ((conteudoFilete >> 1) | conteudoFilete);
         }

         bTmp[0] = (conteudoFilete >> 8) & 0x00FF;
         bTmp[1] = (conteudoFilete) & 0x00FF;
      }
   }
   else {
     bTmp[0] = 0;
     bTmp[1] = 0;
   }
}

/************************************************************************************************************/
void mtx_Get9x24(uint8_t *matrizChar, st_prtcmd_fontAlt *strFont, char character, uint8_t filletNum, uint8_t *bTmp, Bool negrito)
/************************************************************************************************************/
{
   uint16_t conteudoFilete = 0;
   uint8_t caractereNaoSinalizado = (uint8_t) character;

   if (!(((caractereNaoSinalizado >= 0) && (caractereNaoSinalizado <= 32u))
       || ((caractereNaoSinalizado >=  127u) && (caractereNaoSinalizado <= 160u))))
   { // É um caractere com conteúdo definido?
      if (filletNum < 24u) // É um filete válido?
      {

         uint16_t desloc = ((uint16_t)(caractereNaoSinalizado - ((caractereNaoSinalizado > 160u)
                  ? (33u + 34u) : 33u))) * 24u;

         /*
          * Ocupa 2 bytes, incluindo 1 bit em branco à esquerda do desenho (MSb - bit mais significativo).
          */
         conteudoFilete = (((uint16_t) matrix8x24Cp819[desloc + ((uint16_t)filletNum)]) << 7) & 0x7F80u;

         if(negrito)
         {
           conteudoFilete = ((conteudoFilete >> 1) | conteudoFilete);
         }

         bTmp[0] = (conteudoFilete >> 8) & 0x00FF;
         bTmp[1] = (conteudoFilete) & 0x00FF;
      }
   }
   else {
     bTmp[0] = 0;
     bTmp[1] = 0;
   }
}


/************************************************************************************************************/
void mtx_Get16x32(uint8_t *matrizChar, st_prtcmd_fontAlt *strFont, char character, uint8_t filletNum, uint8_t *bTmp, Bool negrito)
/************************************************************************************************************/
{
   int16_t conteudoFilete = 0;
   uint8_t caractereNaoSinalizado = (uint8_t) character;

   if (!(((caractereNaoSinalizado >= 0) && (caractereNaoSinalizado <= 32u))
       || ((caractereNaoSinalizado >=  127u) && (caractereNaoSinalizado <= 160u))))
   { // É um caractere com conteúdo definido?
      if (filletNum < 32) // É um filete válido?
      {
         filletNum = (filletNum * 24u) / 32u; // Converte de 32 para 24 para usar a fonte de 24 de altura.

         uint16_t desloc = ((uint16_t)(caractereNaoSinalizado - ((caractereNaoSinalizado > 160u)
                  ? (33u + 34u) : 33u))) * 24u;

         conteudoFilete = matrix12x24Cp819[desloc + ((uint16_t)filletNum)];

         {
            uint8_t pontos[16];

            pontos[0] = ((conteudoFilete  & 0x8000u) != 0) ? 1 : 0;
            pontos[1] = ((conteudoFilete  & 0x8000u) != 0) ? 1 : 0;
            pontos[2] = ((conteudoFilete  & 0x4000u) != 0) ? 1 : 0;
            pontos[3] = ((conteudoFilete  & 0x2000u) != 0) ? 1 : 0;
            pontos[4] = ((conteudoFilete  & 0x1000u) != 0) ? 1 : 0;
            pontos[5] = ((conteudoFilete  & 0x1000u) != 0) ? 1 : 0;
            pontos[6] = ((conteudoFilete  & 0x0800u) != 0) ? 1 : 0;
            pontos[7] = ((conteudoFilete  & 0x0400u) != 0) ? 1 : 0;
            pontos[8] = ((conteudoFilete  & 0x0200u) != 0) ? 1 : 0;
            pontos[9] = ((conteudoFilete  & 0x0200u) != 0) ? 1 : 0;
            pontos[10] = ((conteudoFilete & 0x0100u) != 0) ? 1 : 0;
            pontos[11] = ((conteudoFilete & 0x0080u) != 0) ? 1 : 0;
            pontos[12] = ((conteudoFilete & 0x0040u) != 0) ? 1 : 0;
            pontos[13] = ((conteudoFilete & 0x0040u) != 0) ? 1 : 0;
            pontos[14] = ((conteudoFilete & 0x0020u) != 0) ? 1 : 0;
            pontos[15] = ((conteudoFilete & 0x0010u) != 0) ? 1 : 0;

            conteudoFilete = 0;

            for (uint8_t i = 0; i < 16; i++)
            {
               if (pontos[i] != 0)
               {
                  conteudoFilete |= 0x8000u >> i;
               }
            }
         }

        if(negrito)
        {
          conteudoFilete = ((conteudoFilete >> 1) | conteudoFilete);
        }

        bTmp[0] = (conteudoFilete >> 8) & 0x00FF;
        bTmp[1] = (conteudoFilete) & 0x00FF;
      }
   }
   else {
     bTmp[0] = 0;
     bTmp[1] = 0;
   }
}

void double_filete(uint8_t * filetein, uint8_t * fileteout, uint8_t nBits)
{
   memset(fileteout, 0x00, ceil((double)nBits/8)*2);

   uint8_t mask_in = 0x80;
   uint8_t mask_out = 0x80;

   for(uint16_t ii = 0; ii < nBits; ii++)
   {
     if(*filetein & mask_in)
     {
       *fileteout |= mask_out;
       mask_out >>= 1;
       *fileteout |= mask_out;
       mask_out >>= 1;
     }
     else
     {
       mask_out >>= 2;
     }

     if(mask_out == 0)
     {
       mask_out = 0x80;
       fileteout++;
     }

     mask_in >>=1;
     if(mask_in == 0)
     {
       mask_in = 0x80;
       filetein++;
     }
   }
}

/*  ========================================================================================
     Fnc: uint16_t lib_prtcmd_checkReady(char *PrtCmdMsg) ;
     ================================ */
  /**
  @brief Verifica se a impressora está pronta

  @details    Esta função verifica se a impressora está pronta para imprimir ou fazer o corte do papel
              Para isto, ela verifica se tem papel, se está fechada ou se está com energia suficiente.

  @param none

  @return 0 se não tiver erro, número de bytes inseridos no buffer de mensagem

  @author Ie

  @warning

  */
uint16_t lib_prtcmd_checkReady( uint8_t *PrtCmdMsg)
{
  if( !lib_is_PE_ok())
  {
//    PrtCmdMsg[COM_STATUS] = MIV_ERROR_MIV_NOT_READY;
    StatusResp = MIV_ERROR_MIV_NOT_READY;
    return 3;
  }

  if( !lib_is_HDUP_ok())
  {
//    PrtCmdMsg[COM_STATUS] = MIV_ERROR_MIV_NOT_READY;
    StatusResp = MIV_ERROR_MIV_NOT_READY;
    return 3;
  }

  if( !lib_is_mon8v5_ok())                                                  // se supercaps ainda carregando...
  {
    unsigned int SCvalue = lib_get_SuperCaps_value();
//    PrtCmdMsg[COM_STATUS] = MIV_ERROR_MIV_NOT_READY;
   StatusResp = MIV_ERROR_MIV_NOT_READY;
    sprintf( (char*)&PrtCmdMsg[RES_DADOS], "%04u", SCvalue);
    return 3 + 4;
  }
  return 0;
}


