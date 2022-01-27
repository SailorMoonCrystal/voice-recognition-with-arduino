import os
import sys
import time
import serial
import librosa
import numpy as np
from keras.models import load_model


def extract_features():
    X, sample_rate = librosa.load('nagrania/nagranie.wav', res_type='kaiser_fast')

    mfccs = np.mean(librosa.feature.mfcc(y=X, sr=sample_rate, n_mfcc=40).T, axis=0)

    stft = np.abs(librosa.stft(X))

    chroma = np.mean(librosa.feature.chroma_stft(S=stft, sr=sample_rate).T, axis=0)

    mel = np.mean(librosa.feature.melspectrogram(X, sr=sample_rate).T, axis=0)

    contrast = np.mean(librosa.feature.spectral_contrast(S=stft, sr=sample_rate).T, axis=0)

    tonnetz = np.mean(librosa.feature.tonnetz(y=librosa.effects.harmonic(X), sr=sample_rate).T, axis=0)

    return mfccs, chroma, mel, contrast, tonnetz


class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            else:
                self.buf.extend(data)

ser = serial.Serial(port='COM5', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)

# Odczytanie modelu
model = load_model('saved_model')

line = ReadLine(ser)


while True:
    string = bytes(line.readline())
    print(string)

    if str(string).startswith("b'RIFF") or str(string).startswith('b"RIFF'):
        f=open("nagrania/nagranie.wav", 'wb')
        print('file created')
        f.write(bytes(string))
        f.close()
        print('file saved')
        break

if os.path.exists('nagrania/nagranie.wav'):
    with open('nagrania/nagranie.wav', 'r') as f:
        extract = extract_features()

    features = [np.concatenate((extract[0], extract[1], extract[2], extract[3], extract[4]), axis=0)]
    prediction = model.predict(np.array(features))
    classes = np.argmax(prediction, axis=1)

    #print(prediction)

    if prediction[0][0] >= 0.95:
        ser.write(b'1')
        print('Łukasz')
    elif prediction[0][1] >= 0.95:
        ser.write(b'2')
        print('Szymon')
    elif prediction[0][2] >= 0.95:
        ser.write(b'3')
        print('Krzysztof')
    elif prediction[0][3] >= 0.95:
        ser.write(b'4')
        print('Kristina')
    else:
        ser.write(b'5')
        print('nieznane')
    os.remove('nagrania/nagranie.wav')

ser.close()
