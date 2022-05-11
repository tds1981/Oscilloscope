#include "wav.h"

WAVHEADER WavHeader(uint32_t  sampleRate, uint32_t SizeData)
{
    WAVHEADER Wheader;
    for(int8_t i=0; i<4; i++)
    {
        Wheader.chunkId[i]=static_cast<char>(0x52494646>>(3-i)*8); //"RIFF"
        Wheader.format[i]=static_cast<char>(0x57415645>>(3-i)*8);  //"WAVE"
        Wheader.subchunk1Id[i]=static_cast<char>(0x666d7420>>(3-i)*8);
        Wheader.subchunk2Id[i]=static_cast<char>(0x64617461>>(3-i)*8); //«data»
    }
     if (SizeData!=0) Wheader.chunkSize = SizeData-8;
     Wheader.subchunk1Size = 16;
     Wheader.audioFormat = 1; // Для PCM = 1 (то есть, Линейное квантование)
     Wheader.numChannels = 1;
     Wheader.sampleRate = sampleRate; //Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
     Wheader.blockAlign = 2; //Количество байт для одного сэмпла, включая все каналы.
     Wheader.bitsPerSample = 16;
     Wheader.byteRate = sampleRate * Wheader.numChannels * (Wheader.bitsPerSample/8); //Количество байт, переданных за секунду воспроизведения.
     if (SizeData!=0) Wheader.subchunk2Size = SizeData-44; //Количество байт в области данных.
    return Wheader;
}
