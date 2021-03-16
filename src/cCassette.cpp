#include "cCassette.h"
//#include <ff_utils.h>
#include "cfileinfo.h"
#include "cutils.h"
#include <cstring>
#include "cRtc.h"
#define DEBUG_LEVEL 1
#include "debug.h"

#ifndef ARDUINO_TEENSY41
extern "C" uint32_t usd_getError(void);
#endif   //#ifndef ARDUINO_TEENSY41

void cCassette::setSamplingRate(uint32_t s) {
  m_sampleRate = s;
  m_player.setSampleRate(s);
}

int cCassette::startRec(float recTime) {
  m_recordingTime = recTime;
//  Serial.print("startRecording");
  if (m_isRecFileOpen)
  {
    
    enSdRes rc = cSdCard::inst().closeFile(m_fil);
    if (rc != OK) 
      return 1;
    m_isRecFileOpen = false;
  }

  if (!m_isRecFileOpen) {
    enSdRes ret = createRecordingDir();
    if(ret == OK) {
      char buf[16];
      snprintf(buf,sizeof(buf),"/%02i%02i.raw",m_min, m_sec);
      strcat(m_fileName, buf);
      return startRec();
    }
  }
  return 0;
}


int cCassette::startRec(const char* name, float recTime) {
  m_recordingTime = recTime;
  strcpy(m_fileName, name);
  return startRec();
}

int cCassette::startRec() {
 // TCHAR wfilename[80];

  
  m_mode = CAS_REC;
  cSdCard& sd = cSdCard::inst();
//      Serial.println(m_fileName);
//  char2tchar(m_fileName, 80, wfilename);
  //m_rc = f_stat (wfilename, 0);
  enSdRes rc = sd.openFile(m_fileName, m_fil, WRITE);
//    Serial.printf("stat %d %x\n", m_rc, m_fil.obj.sclust);
//  m_rc = f_open (&m_fil, wfilename, FA_WRITE | FA_CREATE_ALWAYS);
//    Serial.printf(" opened %d %x\n\r", m_rc, m_fil.obj.sclust);

    // check if file is Good
  if (rc != OK) { // only option is to close file
    sd.closeFile(m_fil);
//    m_rc = f_close(&m_fil);
//    if (m_rc == FR_INVALID_OBJECT) {
//        Serial.println("unlinking file");
//      m_rc = f_unlink(wfilename);
 //     if (m_rc) 
 //       return m_rc;
  //    return OPEN_ERR;
  //  }
   // else
      return 1;
  }
  /*    // retry open file
  m_rc = f_open(&m_fil, wfilename, FA_WRITE | FA_CREATE_ALWAYS);
  if (m_rc) 
    return m_rc;
  m_isRecFileOpen = true;

#ifdef WAV
  writeWavHeader();
#endif
*/
  m_timer.start();
  m_recorder.begin();
  return 0;
}


int cCassette::operate(enCassMode& mode) {
  enSdRes rc = OK;
  if (m_mode == CAS_STOP) {
    mode = m_mode;    
    return 0;
  }

  else if (m_mode == CAS_REC) {
    #define N_BUFFER 2
    #define N_LOOPS (BUFF * N_BUFFER)
  // buffer size total = 256 * n_buffer * n_loops
  // queue: write n_buffer blocks * 256 bytes to buffer at a time; free queue buffer;
  // repeat n_loops times ( * n_buffer * 256 = total amount to write at one time)
  // then write to SD card

    if (m_recorder.available() >= N_BUFFER  ) {// one buffer = 256 (8bit)-bytes = block of 128 16-bit samples
      for (int i = 0; i < N_BUFFER; i++) {
        memcpy(m_buffern + i * 256 + m_nj * 256 * N_BUFFER, m_recorder.readBuffer(), 256);
        m_recorder.freeBuffer();
      }

      m_nj ++;

      if (m_nj >=  (N_LOOPS - 1)) {
        m_nj = 0;
        
//        m_rc = f_write (&m_fil, m_buffern, N_BUFFER * 256 * N_LOOPS, &m_wr);
        cSdCard& sd = cSdCard::inst();
        rc = sd.writeFile (m_fil, m_buffern, m_wr, N_BUFFER * 256 * N_LOOPS);
        if (rc != OK) { // IO error
          m_mode = CAS_STOP;
          mode = m_mode;
          return 1;
        }
      }
    }
    
    if(m_timer.runTime() > m_recordingTime)
      stop();
    return 0;
  }

  else if(m_mode == CAS_PLAY) {
    if (!m_player.isPlaying()) {
      m_player.stop();
      DPRINTLN1("File is over");
      m_mode = CAS_STOP;
    }
  } 
  mode = m_mode; 
  return 0;
}


int cCassette::stop() {
  enSdRes rc = OK;
  if (m_mode == CAS_REC) {
//    Serial.print("stopRecording");
    m_recorder.end();
    cSdCard& sd = cSdCard::inst();
    while (m_recorder.available() > 0) {
//      m_rc = f_write (&m_fil, (byte*)m_recorder.readBuffer(), 256, &m_wr);
      //      frec.write((byte*)recorder.readBuffer(), 256);
      rc = sd.writeFile(m_fil, (byte*)m_recorder.readBuffer(), m_wr, 256);
      m_recorder.freeBuffer();
    }
#ifdef WAV
    finalizeWavFile();
#endif
    //close file
    rc = sd.closeFile(m_fil);
    writeInfoFile();
    if (rc)
      return 1;
    //
    m_isRecFileOpen = false;
    DPRINTLN1(" Recording stopped!");
  }
  
  else if(m_mode == CAS_PLAY) {
    DPRINTLN1("stopPlaying");
    m_player.stop();
  }
  m_mode = CAS_STOP;
  return 0;
}


void cCassette::startPlay() {
  DPRINTLN1("startPlaying ");
  DPRINTF1("Playfile: %s\n", m_fileName);
  delay(100);
  m_player.play(m_fileName);
  m_mode = CAS_PLAY;
}

void cCassette::writeWavHeader() {
  //https://www.cplusplus.com/forum/beginner/166954/
  char buf[24];
  memcpy(buf,"RIFF----WAVEfmt ", 16);     // (chunk size to be filled in later)
  //f_write(&m_fil, buf,16, &m_wr);
  cSdCard::inst().writeFile(m_fil, buf, m_wr, 16);
  writeWord(16, 4 );  // no extension data
  writeWord( 1, 2 );  // PCM - integer samples
  writeWord( 1, 2 );  // one channel (mono file)
  writeWord( m_sampleRate, 4 );  // samples per second (Hz)
  writeWord( m_sampleRate * 2, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  writeWord( 2, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
  writeWord(16, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  memcpy(buf,"data----", 8);     // (chunk size to be filled in later)
//  f_write(&m_fil, buf,8, &m_wr);
  cSdCard::inst().writeFile(m_fil, buf, m_wr, 8);
}


void cCassette::writeWord(uint32_t value, size_t size) {
  for (; size; --size, value >>= 8) {
    byte val = value & 0xFF;    
//    f_write(&m_fil, &val, 1, &m_wr);
    cSdCard::inst().writeFile(m_fil, &val, m_wr, 1);
//    &m_fil.put( static_cast <char> (value & 0xFF) );
  }
}

void cCassette::finalizeWavFile() {
  /* TODO
   // (We'll need the final file size to fix the chunk sizes above)
  //size_t file_length = f.tellp();
  size_t file_length = f_tell(&m_fil);
  // Fix the data chunk header to contain the data size
//  f.seekp( data_chunk_pos + 4 );
  f_lseek(&m_fil, WAV_DATACHUNK_POS + 4);
  writeWord(file_length - WAV_DATACHUNK_POS + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  f_lseek(&m_fil, 0 + 4);
  //f.seekp( 0 + 4 );
  writeWord(file_length - 8, 4 ); 
  */
}


void cCassette::writeInfoFile()
{
  cFileInfo info;
  char date [32];
  char infoFile[80];
 
  strncpy(infoFile, m_fileName, sizeof(infoFile));
  cUtils::replace(m_fileName, ".raw", ".xml", infoFile, sizeof(infoFile));
  
  snprintf(date,sizeof(date),"%02i.%02i.%02i %02i:%02i:%02i",m_day, m_month, m_year, m_hour, m_min, m_sec);
  info.write(infoFile, m_recordingTime, m_sampleRate, date, cUtils::getFileName(m_fileName)); 
}

enSdRes cCassette::createRecordingDir()
{
  char buf[80];

  m_year = year();
  m_month = month();
  m_day = day();
  m_hour = hour();
  m_min = minute();
  m_sec = second();

  snprintf(m_fileName, sizeof(m_fileName),"0:/rec/%02i/%02i/%02i/%02i",m_year, m_month, m_day, m_hour);
  enSdRes ret = cSdCard::inst().chdir(m_fileName);
  if(ret != OK) {
    ret = cSdCard::inst().chdir("0:/rec");
    if(ret != OK) {
      ret = cSdCard::inst().mkDir(buf);
    }
    snprintf(buf, sizeof(buf),"0:/rec/%02i",m_year);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK) {
      ret = cSdCard::inst().mkDir(buf);
    }
    snprintf(buf, sizeof(buf),"0:/rec/%02i/%02i", m_year, m_month);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK) {
      ret = cSdCard::inst().mkDir(buf);
    }      
    snprintf(buf, sizeof(buf),"0:/rec/%02i/%02i/%02i",m_year ,m_month ,m_day);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK) {
      ret = cSdCard::inst().mkDir(buf);
    }      
    snprintf(buf, sizeof(buf),"0:/rec/%02i/%02i/%02i/%02i",m_year ,m_month, m_day, m_hour);
    ret = cSdCard::inst().chdir(buf);
    if(ret != OK) {
      ret = cSdCard::inst().mkDir(buf);
    }      
    ret = cSdCard::inst().chdir(m_fileName);
  }
  return ret;
}
