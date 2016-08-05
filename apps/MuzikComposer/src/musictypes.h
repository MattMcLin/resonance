#include <vector>
#include <memory>
#include <map>

using namespace std;

struct TimeSignature
{
    int beats;
    int divisor;
};

static char* degreeNamesMajor[13][7] = {
    {"Db", "Eb", "F", "Gb", "Ab", "Bb", "C"},
    {"Ab", "Bb", "C", "Db", "Eb", "F", "G"},
    {"Eb", "F", "G", "Ab", "Bb", "C", "D"},
    {"Bb", "C", "D", "Eb", "F", "G", "A"},
    {"F", "G", "A", "Bb", "C", "D", "E"},
    {"C", "D", "E", "F", "G", "A", "B"},
    {"G", "A", "B", "C", "D", "E", "F#"},
    {"D", "E", "F#", "G", "A", "B", "C#"},
    {"A", "B", "C#", "D", "E", "F#", "G#"},
    {"E", "F#", "G#", "A", "B", "C#", "D#"},
    {"B", "C#", "D#", "E", "F#", "G#", "A#"},
    {"F#", "G#", "A#", "B", "C#", "D#", "E#"},
    {"C#", "D#", "E#", "F#", "G#", "A#", "B#"},
};

static char* degreeNamesMinor[13][7] = {
    { "Db", "Eb", "Fb", "Gb", "Ab", "Bbb", "Cb" },
    { "Ab", "Bb", "Cb", "Db", "Eb", "Fb", "Gb" },
    { "Eb", "F", "Gb", "Ab", "Bb", "Cb", "Db" },
    { "Bb", "C", "Db", "Eb", "F", "Gb", "Ab" },
    { "F", "G", "Ab", "Bb", "C", "Db", "Eb" },
    { "C", "D", "Eb", "F", "G", "Ab", "Bb" },
    { "G", "A", "Bb", "C", "D", "Eb", "F" },
    { "D", "E", "F", "G", "A", "Bb", "C" },
    { "A", "B", "C", "D", "E", "F", "G" },
    { "E", "F#", "G", "A", "B", "C", "D" },
    { "B", "C#", "D", "E", "F#", "G", "A" },
    { "F#", "G#", "A", "B", "C#", "D", "E" },
    { "C#", "D#", "E", "F#", "G#", "A", "B" },
};

enum KeyRoot
{
    key_Dflat,
    key_Aflat,
    key_Eflat,
    key_Bflat,
    key_F,
    key_C,
    key_G,
    key_D,
    key_A,
    key_E,
    key_B,
    key_Fsharp,
    key_Csharp,
    MaxKeyRoots
};

static int NumberOfKeys = static_cast<int>(MaxKeyRoots);

struct Key
{
    KeyRoot root;
    bool isMajor;
};

enum ChordType
{
    chord_maj,
    chord_min,
    chord_maj7,
    chord_min7,
    chord_maj9,
    chord_min9,
    chord_maj7b5,
    chord_min7b5,
    chord_sus2,
    chord_sus4,
    chord_dim,
    chord_dim7,
    chord_aug,
};

ChordType keyChordsMajor[] = { chord_maj, chord_min, chord_min, chord_maj, chord_maj, chord_min, chord_dim };
ChordType keyChordsMinor[] = { chord_min, chord_dim, chord_maj, chord_min, chord_min, chord_maj, chord_maj };
static map<ChordType, ChordType> biggerChords;   // maps "smaller" chords to "bigger" chords

/*
TODO:
Concept of recursive subdivision.  Start with a single chord/key.  Subdivide into 2 - 4 chords which
lead into the original chord.  Then for each of those, subdivide again.  Always pick chords that lead into
the starting chord.
*/
struct ChordResolution
{
    int initialDegree;
    ChordType chord;
    int nextDegree;
};

struct MusicChord
{
    ChordType chordType;
    int degree;
    int inversion;
    float beats;
};

class Measure
{

};

class Section
{
    int measures;
    Key key;
    vector<MusicChord> chords;
};

class Structure
{
    TimeSignature timeSignature;
    vector<shared_ptr<Section>> sections;
};
