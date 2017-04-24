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

    // Send program change

    message.push_back(201);
    message.push_back(0);

    midiout->sendMessage(&message);
    message.clear();

    // Note off

    //message.push_back(137);
    //message.push_back(0);
    //message.push_back(0);

    //midiout->sendMessage(&message);
    //message.clear();

    // Play pre-click

    for (uint s = 0; s < accBeat; s++) {
        if (!playing) {
            break;
        }

        // Note on

        message.push_back(153);

        if (s % accBeat == 0) {
            // Upbeat
            message.push_back(56);
        } else {
            // Downbeat
            message.push_back(37);
        }

        // Volume

        message.push_back(80);

        midiout->sendMessage(&message);
        message.clear();

        // Wait a beat
        msleep(60.0 / float(currentSpeed / 4) * 250.0);
    }

    // Play

    for (uint s = 0; s < length * accBeat; s++) {
        if (!playing) {
            break;
        }

        // Note on

        message.push_back(153);

        if (s % accBeat == 0) {
            // Upbeat
            message.push_back(56);
        } else {
            // Downbeat
            message.push_back(37);
        }

        // Volume

        message.push_back(127);

        midiout->sendMessage(&message);
        message.clear();

        // Wait a beat
        msleep(60.0 / float(currentSpeed / 4) * 250.0);
    }

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
    midiout->openPort(nPorts - 1);

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

    // Set Volume for all channels

    for (int x = 0; x < 16; x++) {
        // Control Change: 176, 7, 100 (volume)
        message.push_back(176 + x); // Channel 1 to 16
        message.push_back(7);
        message.push_back(pVolume);
        midiout->sendMessage(&message);
        message.clear();
    }
}

void Midi::play()
{
    play(currentSpeed, beats, accBeat, 4);
}

void Midi::play(quint16 pSpeed, uint pBeats, uint pAccBeat, uint pLength)
{
    currentSpeed = pSpeed;
    currentStep = 0;

    beats = pBeats;
    accBeat = pAccBeat;
    length = pLength;

    playing = true;

    // Start Thread
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
        play();
    } else {
        playing = false;
        silence();
        emit paused();
    }
}

void Midi::silence()
{
    // Send NoteOff for everything

    std::vector<unsigned char> message;

    // Channels
    for (int c = 0; c < 16; c++) {

        // Notes
        for (int n = 0; n < 128; n++) {

            // Control Change: 176, 7, 100 (volume)
            message.push_back(128 + c); // Channel 1 to 16
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
