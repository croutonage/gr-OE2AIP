#include "dstar_header_decoder.h"
#include <string.h>

void headerDecode(unsigned char raw_header[660], bool hardDecition, header_t *decodedHeader)
{

	/*for ( int i = 0; i < 660; i++){
		//cout << hex << (int)raw_header[i] << ", ";
		raw_header[i] = raw_header_data[i];
	}*/


	// Descrambling der empfangenen Symbole
	char sr = 0x7f;
	for (short i=0; i<660; ++i) {
	  if ( ((sr>>3)&0x1) ^ (sr&0x1) ){
	    sr >>= 1;
	    sr  |= 64;
	    raw_header[i] ^= 1;
	  } else {
	    sr >>= 1;
	  }
	}
       
        // Das ist Array fuer die deinterleavte Symbole
        char symbole2[660];
        
        for (short i = 0; i < 12; ++i) {
          for (short j=0; j < 28; ++j) {
            symbole2[i + j*24] = raw_header[i*28 + j];
          }
        }
        
        for (short i = 12; i < 24; ++i) {
          for (short j=0; j < 27; ++j) {
            symbole2[i + j*24] = raw_header[i*27 + j + 12];
          }
        }
       
       
		if (hardDecition)
		{
			// Taeusche eine "Soft-Decision"-Dekodierung vor,
			// indem "0" --> "0" bzw. "1" --> "31" ersetzt wird.
			for (short i=0; i<660; ++i)
			  symbole2[i] *= 31;
        }
        
        // ===============================================================================
        // ================                                   ============================
        // ================  Viterbi-Decodierung nach DL3OCK  ============================
        // ================                                   ============================
        // ===============================================================================
        
        // Eine feinere Quantisierung als mit 5Bit bringt keine nenneswerte
        // Verbesserung der Korrekturfaehigkeit des Dekoders.
        
        // Analoge Werte fuer die Referenzsymbole
        const char  High = 31;  // fuer die "5Bit-Quantisierung".
        const char  Low  =  0;
        const short KillerMetrik = 400;
        
        // Zustanduebergangsmatrix. Sie wird in der
        // vorliegenden Implementierung nicht benoetigt!
        //  char    T[4][2] = {{0, 1},
        //                     {2, 3},
        //                     {0, 1},
        //                     {2, 3}};
        
        // inverse Zustanduebergangsmatrix
        char invT[4][3] = {{0, 2, 0},
                           {0, 2, 1},
                           {1, 3, 0},
                           {1, 3, 1}};
        
        // Ausgabematrix fuer den ersten Bit als analoger Wert
        char A1[4][2] = {{Low,  High},
                         {High, Low},
                         {High, Low},
                         {Low,  High}};
        
        // Ausgabematrix fuer den zweiten Bit als analoger Wert
        char A2[4][2] = {{Low,  High},
                         {Low,  High},
                         {High, Low},
                         {High, Low}};
        // Metrik
        int Metrik[4];
        int tempMetrik[4];
        
        // Alternierende Speicherfelder fuer die dekodierten Daten
        char Datenfolge_plus [4][330];
        char Datenfolge_minus[4][330];
        
        short datenbuffer = 1;
        int Metrik_A, Metrik_B;
        short j0 = 0;
        
        // Initialisiere die Anfangsmetriken
        Metrik[0] = 0;
        Metrik[1] = KillerMetrik;
        Metrik[2] = KillerMetrik;
        Metrik[3] = KillerMetrik;
        
        for (short k=0; k<330; ++k){
          register char Symbol1 = symbole2[2*k];
          register char Symbol2 = symbole2[2*k+1];
          register short temp1, temp2;
          
          for (char S=0; S<4; ++S){
               temp1 = Symbol1-A1[ invT[S][0] ][ invT[S][2] ];
               temp2 = Symbol2-A2[ invT[S][0] ][ invT[S][2] ];
            Metrik_A = Metrik[invT[S][0]] + temp1*temp1 + temp2*temp2;
            
               temp1 = Symbol1-A1[ invT[S][1] ][ invT[S][2] ];
               temp2 = Symbol2-A2[ invT[S][1] ][ invT[S][2] ];
            Metrik_B = Metrik[invT[S][1]] + temp1*temp1 + temp2*temp2;
            
            if (Metrik_A < Metrik_B){
              tempMetrik[S] = Metrik_A;
              if (datenbuffer>0) {
                for (short j=j0; j<k; ++j){
                  Datenfolge_plus[S][j] = Datenfolge_minus[invT[S][0]][j];
                }
                Datenfolge_plus[S][k] = invT[S][2];
              } else {
                for (short j=j0; j<k; ++j){
                  Datenfolge_minus[S][j] = Datenfolge_plus[invT[S][0]][j];
                }
                Datenfolge_minus[S][k] = invT[S][2];
              }
            } else {
              tempMetrik[S] = Metrik_B;
              if (datenbuffer>0) {
                for (short j=j0; j<k; ++j){
                  Datenfolge_plus[S][j] = Datenfolge_minus[invT[S][1]][j];
                }
                Datenfolge_plus[S][k] = invT[S][2];
              } else {
                for (short j=j0; j<k; ++j){
                  Datenfolge_minus[S][j] = Datenfolge_plus[invT[S][1]][j];
                }
                Datenfolge_minus[S][k] = invT[S][2];
              }
            }
          }
          
          // kopiere die temp-Metriken zurueck
          for (char i=0; i<4; ++i){
            Metrik[i] = tempMetrik[i];
          }
          
          // Erfahrungsgemaess stimmen alle Pfade bis auf den letzen relativ
          // kurzen Stueck ueberein (s.g. Einschwingphaenomen). Deshalb reicht
          // es auch aus, nur die aktuelsten (hier ca. 20) Elemente umzukopieren.
          // Mit 30, basierend auf meinen Beobachtungen, ist man auf jeden Fall auf
          // der sicheren Seite.
          if (k>29){
            j0 = k-30;
          }
          
          datenbuffer *= -1;
        }
        
        // Array fuer Header in Byte-Format
        char header[41];
        
        // Konvertiere die Bits in Bytes und speichere die
        // empfangene 41 Header-Bytes ab.
        for (char i=0; i<41; ++i){
          register char zeichen = 0;
          
          if ( Datenfolge_minus[0][i*8  ] ) zeichen += (1<<0);
          if ( Datenfolge_minus[0][i*8+1] ) zeichen += (1<<1);
          if ( Datenfolge_minus[0][i*8+2] ) zeichen += (1<<2);
          if ( Datenfolge_minus[0][i*8+3] ) zeichen += (1<<3);
          if ( Datenfolge_minus[0][i*8+4] ) zeichen += (1<<4);
          if ( Datenfolge_minus[0][i*8+5] ) zeichen += (1<<5);
          if ( Datenfolge_minus[0][i*8+6] ) zeichen += (1<<6);
          if ( Datenfolge_minus[0][i*8+7] ) zeichen += (1<<7);
          
          header[i] = zeichen;
        }

        if ( header[0] & (1<<7) )
			decodedHeader->isData = true;
		else
			decodedHeader->isData = false;
        
        if ( header[0] & (1<<6) )
			decodedHeader->viaRepeater = true;
        else
			decodedHeader->viaRepeater = false;
        
        if ( header[0] & (1<<5) )
			decodedHeader->interruption = true;
        else
			decodedHeader->interruption = false;
        
        if ( header[0] & (1<<4) )
			decodedHeader->isControlSignal = true;
        else
			decodedHeader->isControlSignal = false;
        
        if ( header[0] & (1<<3) )
			decodedHeader->emergency = true;
        else
			decodedHeader->emergency = false;

		// Copy Flag1
		memcpy(&decodedHeader->flag1, &header[0], 1);
		
		// Copy Flag2
		memcpy(&decodedHeader->flag2, &header[1], 1);
		
		// Copy Flag3
		memcpy(&decodedHeader->flag3, &header[2], 1);
       
		// Copy RPT2
		memcpy(&decodedHeader->destinationRepeater, &header[3], 8);
       
		// Copy RPT1
		memcpy(&decodedHeader->departureRepeater, &header[11], 8);
		
		// Copy companion Callsign
		memcpy(&decodedHeader->companionCallsign, &header[19], 8);
		
		// Copy own Callsign
		memcpy(&decodedHeader->ownCallsign, &header[27], 8);
		
		// Copy own Callsign suffix
		memcpy(&decodedHeader->ownCallsignSuffix, &header[35], 4);
		
		// Copy CRC
		memcpy(&decodedHeader->checksum, &header[39], 2);
	
        // Berechne CRC vom ganzen Header
        //
        // Generatorpolynom G(x) = x^16 + x^12 + x^5 + 1
        // ohne die fuehrende 1 UND in umgekehrter Reihenfolge
        const unsigned short genpoly = 0x8408;
        
        unsigned short crc = 0xffff;
        
        for (char i=0; i<39; ++i){
          crc ^= header[i];
          for (char j=0; j<8; ++j){
            if ( crc & 0x1 ) {
              crc >>= 1;
              crc ^= genpoly;
            } else {
              crc >>= 1;
            }
          }
        }
        
        // Beachte die Reihenfolge der CRC-Bytes!!!
        // Zunaechst kommt Low- und dann High-Byte
        // in "LSB first" Reihenfolge.
        crc ^= 0xffff;        // invertiere das Ergebnis
        
        decodedHeader->calculatedChecksum = crc;
        
      /*  
        if ( (0xffff&(header[40]<<8))+(header[39]&0xff) == crc )
	  cout << "OK" << endl;
        else
	  cout << "NOK" << endl;
*/
}
