int incomingByte = 0;

const int gridSize = 4;
const int startingPin = 2;

int row[gridSize];
int col[gridSize];

int rgbArray[7][3] = {
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 },
  { 1, 1, 0 },
  { 0, 1, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 }
};

class Led{
  private:
  int _anodePin;
  int _cathodePin;

  void setOn () {
    digitalWrite(_anodePin, HIGH); // the anode is high
    digitalWrite(_cathodePin, LOW);   // the cathode is low
  }

  void setOff () {
    digitalWrite(_anodePin, LOW);  // the anode is low
    digitalWrite(_cathodePin, HIGH);  // the cathode is high
  }

  public:
  Led () {};

  Led (int anodePin, int cathodePin) {
    _anodePin = anodePin;
    _cathodePin = cathodePin;
  }

  void setSingleColor (int colorValue) {
    (colorValue) ? setOn() : setOff();
    // this turn the led on now we have to let the led on for some time .... 1 microsecond is enough
    delayMicroseconds(1);

    //now we have tu turn it off so we will inverse the polarity
    (colorValue) ? setOff() : setOn();
    //we have turn the led off, next delay it 1 microsecond and  go to the next row
    delayMicroseconds(1);
  }

  void set (int red, int green, int blue) {
    setSingleColor(red);
    setSingleColor(blue);
    setSingleColor(green);
  }
};

class Row {
  int _xCoord;

  public:
  Row () {};
  Row (int xCoord) {
    _xCoord = xCoord;
  };

  Led getLed (int yCoord) {
    Led ledInstance(row[_xCoord], col[yCoord]);
    return ledInstance;
  }

  void clean () {
    set(0, 0, 0);
  }

  void set (int red, int green, int blue) {
    for (int i=0; i < gridSize; i++) {
      getLed(i).set(red, green, blue);
    }
  }
};

class Column {
  int _yCoord;

  public:
  Column () {};
  Column (int yCoord) {
    _yCoord = yCoord;
  };

  Led getLed (int xCoord) {
    Led ledInstance(row[xCoord], col[_yCoord]);
    return ledInstance;
  }

  void clean () {
    set(0, 0, 0);
  }

  void set (int red, int green, int blue) {
    for (int i=0; i < gridSize; i++) {
      getLed(i).set(red, green, blue);
    }
  }
};

class Matrix {
  const unsigned int _tempoMultiplier   = 1 000 000; // microseconds (1 sec)
  const unsigned int _animationDuration = 5 000 000; // microseconds (5 sec)

  unsigned int _start = micros();
  unsigned int _red = 0;
  unsigned int _green = 0;
  unsigned int _blue = 0;

  void setStart () {
    _start = micros();
  }

  void setColor () {
    int colorIndex = random(0, 6);
    _red = rgbArray[colorIndex][0];
    _green = rgbArray[colorIndex][1];
    _blue = rgbArray[colorIndex][2];
  }

  public:
  Matrix () {};

  Row getRow (int xCoord) {
    Row rowInstance(xCoord);
    return rowInstance;
  }

  Column getColumn (int yCoord) {
    Column colInstance(yCoord);
    return colInstance;
  }

  void set (int red, int green, int blue) {
    for (int i=0; i < gridSize; i++) {
      getRow(i).set(red, green, blue);
    }
  }

  // Animations
  // ==========

  void runAnimation (char animationName) {
    setStart();
    setColor();

    while (micros() - _start < _animationDuration) {
      switch (animationName) {
        case 'flash':
          flash(_tempoMultiplier);
          break;
        case 'strobe':
          strobe();
          break;

        case 'scan column':
          scanColumn2d();
          break;
        case 'scan column left':
          scanColumnReverse();
          break;
        case 'scan column right':
          scanColumn();
          break;

        case 'scan row':
          scanRow2d();
          break;
        case 'scan row left':
          scanRowReverse();
          break;
        case 'scan row right':
          scanRow();
          break;

        case 'scan diagonal':
          scanDiagonal2d();
          break;
        case 'scan row up':
          scanDiagonalReverse();
          break;
        case 'scan row down':
          scanDiagonal();
          break;

        default:
          clean();
      }
    }
  }

  void clean () {
    set(0, 0, 0);
  }

  void flash (int spacing) {
    set(_red, _green, _blue);
    delayMicroseconds(spacing);
    clean();
    delayMicroseconds(spacing);
  }

  void strobe () {
    int spacing = [1, 2, 4, 8, 16, 32, 64];

    for (var i=0; i < 7; i++) {
      flash(spacing[i]);
      flash(spacing[i]);
    }
  }

  void scanColumn () {
    for (int column=0; column < gridSize; column++) {
      getColumn(column).set(_red, _green, _blue);
      delayMicroseconds(_tempoMultiplier);
      getColumn(column).clean();
    }
  }

  void scanRow () {
    for (int row=0; row < gridSize; row++) {
      getRow(row).set(_red, _green, _blue);
      delayMicroseconds(_tempoMultiplier);
      getRow(row).clean();
    }
  }

  void scanDiagonal () {
    for (int row=0; row < gridSize; row++) {
      for (int col=0; col < gridSize; col++) {
        if (row == col) {
          getRow(row).getLed(col).set(_red, _green, _blue);
          delayMicroseconds(_tempoMultiplier);
          getRow(row).clean();
        }
      }
    }
  }

  void scanColumnReverse () {
    for (int col=0; col < gridSize; col++) {
      getColumn(7 - col).set(_red, _green, _blue);
      delayMicroseconds(_tempoMultiplier);
      getColumn(7 - col).clean();
    }
  }

  void scanRowReverse () {
    for (int row=0; row < gridSize; row++) {
      getRow(7 - row).set(_red, _green, _blue);
      delayMicroseconds(_tempoMultiplier);
      getRow(7 - row).clean();
    }
  }

  void scanDiagonalReverse () {
    for (int row=0; row < gridSize; row++) {
      for (int col=0; col < gridSize; col++) {
        if (row == col) {
          getRow(7 - row).getLed(7 - col).set(_red, _green, _blue);
          delayMicroseconds(_tempoMultiplier);
          getRow(7 - row).clean();
        }
      }
    }
  }

  void scanColumn2d () {
    scanColumn();
    scanColumnReverse();
  }

  void scanRow2d () {
    scanRow();
    scanRowReverse();
  }

  void scanDiagonal () {
    scanDiagonal();
    scanDiagonalReverse();
  }
};

Matrix matrix;

void setup () {
  for (int pinIndex=0; pinIndex < gridSize; pinIndex++) {
    row[pinIndex] = startingPin + pinIndex;
    col[pinIndex] = startingPin + pinIndex + gridSize;

    // we initialize the pins as output's
    pinMode(col[pinIndex], OUTPUT);
    pinMode(row[pinIndex], OUTPUT);

    // now we have to make the columns high so that all the LED's will be off
    digitalWrite(col[pinIndex], HIGH);
  }
}

void loop () {
  matrix.runAnimation('flash');
  matrix.runAnimation('scan column');
  matrix.runAnimation('scan row');
  matrix.runAnimation('scan diagonal');
  matrix.runAnimation('strobe');
}
