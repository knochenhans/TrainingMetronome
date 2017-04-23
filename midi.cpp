#include "midi.h"

Midi::Midi(QThread* parent) : QThread(parent)
{
    currentStep = 0;
    currentSpeed = 0;

    midiout = 0;

    volume = 0;

    playing = false;
}

Midi::~Midi()
{
    stop();

    wait();

    deinitialize();
}

QStringList Midi::getPorts()
{
    return ports;
}

void Midi::run()
{
    std::vector<unsigned char> message;

    unsigned char channel = 0;
    unsigned char pitch = 0;
    unsigned char programChange = 0;

    // Für das NoteOff für jeweils letzte Note benötigt: für jeden Track die zuletzt gespielte Tonhöhe speichern
    QMap<unsigned int, unsigned char> lastPitch;

    programChange = 201;
    message.push_back(programChange);

    message.push_back(0);

    midiout->sendMessage(&message);
    message.clear();

    message.push_back(137);
    message.push_back(0);
    message.push_back(0);

    midiout->sendMessage(&message);
    message.clear();

    for (int s = 0; s < 100; s++) {
        if (!playing) {
            break;
        }

        message.push_back(153);
        message.push_back(56);
        message.push_back(127);

        midiout->sendMessage(&message);
        message.clear();

        //Note noteUpbeat(56, 127);
        //Note noteDownbeat(37, 127);

        // einen Schlag warten
        msleep(60.0 / float(currentSpeed) * 250.0);
    }

    // Steps durchgehen (erster Track als Längen - Referenz, da ja alle Tracks gleich lang sind!)

    //    for (int s = currentStep; s < currentExercise.getTracks().at(0).getStepCount(); s++) {
    //        if (!playing) {
    //            break;
    //        }

    //        // Aktuellen Schritt für Pause-Funktion zwischenspeichern
    //        currentStep = s;

    //        // Signal für neuen Schritt an Tabber-Anzeige senden
    //        emit newStep(s);

    //        // Tracks durchgehen
    //        for (int t = 0; t < currentExercise.getTracks().size(); t++) {
    //            if (!playing) {
    //                break;
    //            }

    //            // Voices des Tracks an dieser Stelle durchgehen
    //            for (int v = 0; v < currentExercise.getTracks().at(t).getVoiceCount(); v++) {
    //                if (!playing) {
    //                    break;
    //                }

    //                // MIDI-Message für aktuelle Note(n) holen
    //                Note note = currentExercise.getTracks().at(t).getNote(v, s);

    //                if (!note.isEmpty()) {
    //                    // Instrument für aktuelle Note setzen

    //                    // Program change
    //                    if (currentExercise.getTracks().at(t).getInstrument().isPercussion()) {
    //                        // Instrument des aktuellen Tracks ist Percussion → Kanal 10 wählen
    //                        programChange = 201;
    //                    } else {
    //                        // Instrument ist ein normales Instrument → Kanal 1 (erstmal) wählen
    //                        programChange = 192;
    //                    }

    //                    message.push_back(programChange);

    //                    // Instrument
    //                    message.push_back(currentExercise.getTracks().at(t).getInstrument().getMidiProgram());

    //                    midiout->sendMessage(&message);
    //                    message.clear();

    //                    // MIDI-Kanal aus Voice errechnen (max. 16; 10 ist reserviert für Percussion)
    //                    if (currentExercise.getTracks().at(t).getInstrument().isPercussion()) {
    //                        channel = MSG_NOTE_CHAN_10_NOTE_ON;

    //                        // Bei Percussion-Track direkt den Pitch übernehmen
    //                        pitch = currentExercise.getTracks().at(t).getNote(v, s).getPitch();
    //                    } else {
    //                        // TODO: Das mit den Channels erstmal klären. Bis dahin nur einen Channel nutzen (Nummer 2, 1 scheint irgendwie reserviert zu sein.)
    //                        //channel = MSG_NOTE_CHAN_1_NOTE_ON + v;
    //                        channel = MSG_NOTE_CHAN_1_NOTE_ON;
    //                        //if(channel == MSG_NOTE_CHAN_10_NOTE_ON)
    //                        //channel += 1;
    //                        pitch = currentExercise.getTracks().at(t).getInstrument().getMidiPitchFromNote(v, note.getPitch());
    //                    }

    //                    // NoteOff für letzte Note dieses Tracks senden

    //                    // NoteOff für Kanal ist 16 Bits unterhalb von NoteOn
    //                    message.push_back(channel - 16);
    //                    message.push_back(lastPitch.value(t));

    //                    // Volume auf 0 setzen
    //                    message.push_back(0);

    //                    midiout->sendMessage(&message);
    //                    message.clear();

    //                    // NoteOn für aktuelle Note senden

    //                    message.push_back(channel);

    //                    // Tonhöhe aus Instrument und Notenangaben errechnen
    //                    message.push_back(pitch);
    //                    message.push_back(note.getVelocity());

    //                    // letzte Tonhöhe speichern
    //                    lastPitch.insert(t, pitch);

    //                    midiout->sendMessage(&message);
    //                    message.clear();
    //                }
    //            }
    //        }

    //        // einen Schlag warten
    //        msleep(60.0 / float(currentSpeed) * 250.0);
    //    }

    silence();
}

bool Midi::initialize()
{
    midiout = new RtMidiOut();

    std::vector<unsigned char> message;

    // Check available ports.
    unsigned int nPorts = midiout->getPortCount();

    if ( nPorts == 0 ) {
        qDebug() << "No ports available!";

        return false;

    } else {
        for (unsigned int i = 0; i < nPorts; i++) {
            ports << QString::fromStdString(midiout->getPortName(i));
        }
    }

    // Open first available port.
    midiout->openPort(1);

    // Lautstärke

    volume = 127;

    setVolume(volume);

    return true;
}

void Midi::deinitialize()
{
    midiout->closePort();

    delete midiout;
}

void Midi::setVolume(int pVolume)
{
    std::vector<unsigned char> message;

    // Volume für alle Kanäle setzen

    for (int x = 0; x < 16; x++) {
        // Control Change: 176, 7, 100 (volume)
        message.push_back(176 + x); // Channel 1 bis 16
        message.push_back(7);
        message.push_back(pVolume);
        midiout->sendMessage(&message);
        message.clear();
    }
}

void Midi::play(Exercise pExercise, quint16 pSpeed)
{
    currentExercise = pExercise;
    currentSpeed = pSpeed;
    currentStep = 0;

    playing = true;

    // Thread anstoßen
    start(HighestPriority);
}

void Midi::stop()
{
    playing = false;
    currentStep = 0;

    silence();

    emit stopped();
}

void Midi::pause()
{
    if (!playing) {
        play(currentExercise, currentSpeed);
    } else {
        playing = false;
        silence();
        emit paused();
    }
}

void Midi::silence()
{
    // NoteOff übergreifend senden

    std::vector<unsigned char> message;

    // Kanäle
    for (int c = 0; c < 16; c++) {
        // Notenumber
        for (int n = 0; n < 128; n++) {
            // Control Change: 176, 7, 100 (volume)
            message.push_back(128 + c); // Channel 1 bis 16
            message.push_back(n);
            message.push_back(0);
            midiout->sendMessage(&message);
            message.clear();
        }
    }
}

std::vector<unsigned char> Midi::getModeMessage(unsigned int MSG_MODE)
{
    std::vector<unsigned char> message;

    message.push_back(MSG_MODE);
    message.push_back(0);

    return message;
}
