/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/
// legs1.1 slower walking
char motor1Walk[25] =  { 21,  21,  21,  21,  22,  24,  26,  28,  30,  34,  38,  42,  45,  50,  55,  60,  65,  70,  75,  81,  87,  93, 100, 109, 119};
char motor2Walk[25] =  {119, 109, 100,  93,  87,  81,  75,  70,  65,  60,  55,  50,  45,  42,  38,  34,  30,  28,  26,  24,  22,  21,  21,  21,  21};
// center position in the pattern array
unsigned char _centerAbsolute = 12; // (range 8 to 16) bigger the number more weight on front
// static forward ballance
unsigned char _centerStatic = _centerAbsolute;
// char buffer for temporary use
char cBuffer1;
char cBuffer2;
// leg lift angles
unsigned char _liftm = 32;
// points to currentSequence for every leg
unsigned char mainCounter = 0;
// dynamic ballance
allMotors legUp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// 20 positions per sequence
motors sequenceSL[18] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceSR[18] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceFL[18] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceFR[18] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRL[18] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRR[18] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
// motors values for 12 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
allMotors motorValueBuffer = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
allMotors motorValueBufferOld = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
// full cycle
unsigned char fullCycle = 18;
// pointer to array
unsigned char apoint = 0;
// position correction
int correctRoll  = 0;
int correctPitch = 0;

// linear motor movement from point to point
void _updateSequenceLinear(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[fullCycle - 1].motor1;
  cBuffer2 = sequence[fullCycle - 1].motor2;
  sequence[0].motor1 = cBuffer1;
  sequence[0].motor2 = cBuffer2;
  for (i = 1; i < (fullCycle - 1); i++) {
    sequence[i].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * i) / fullCycle;
    sequence[i].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * i) / fullCycle;
  }
  sequence[fullCycle - 1].motor1 = endPosition1;
  sequence[fullCycle - 1].motor2 = endPosition2;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearEnd(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[fullCycle - 1].motor1;
  cBuffer2 = sequence[fullCycle - 1].motor2;
  for (i = 0; i < (fullCycle - 4); i++) {
    sequence[i].motor1 = cBuffer1;
    sequence[i].motor2 = cBuffer2;
  }
  sequence[fullCycle - 4].motor1 = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[fullCycle - 3].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[fullCycle - 2].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[fullCycle - 1].motor1 = endPosition1;
  sequence[fullCycle - 4].motor2 = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[fullCycle - 3].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[fullCycle - 2].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  sequence[fullCycle - 1].motor2 = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[fullCycle - 1].motor1;
  cBuffer2 = sequence[fullCycle - 1].motor2;
  sequence[0].motor1 = cBuffer1;
  sequence[1].motor1 = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[2].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[3].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[0].motor2 = cBuffer2;
  sequence[1].motor2 = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[2].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[3].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  for (i = 4; i < fullCycle; i++) {
    sequence[i].motor1 = endPosition1;
    sequence[i].motor2 = endPosition2;
  }
}

void _updateApointer(unsigned char pointer, unsigned char shift) {
  apoint = pointer + shift;
  if (apoint >= fullCycle) {
    apoint -= fullCycle;
  }
}

// create stand walk sequence
void _updateSequenceStandWalk(char m1, char m2, motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1 - _liftm;
  sequence[apoint].motor2 = m2 - _liftm;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1 - _liftm / 2;
  sequence[apoint].motor2 = m2 - _liftm / 2;
  for (i = 2; i < (fullCycle - 2); i++) {
    _updateApointer(i, shift);
    sequence[apoint].motor1 = m1;
    sequence[apoint].motor2 = m2;
  }
  _updateApointer(fullCycle - 2, shift);
  sequence[apoint].motor1 = m1 - _liftm / 2;
  sequence[apoint].motor2 = m2 - _liftm / 2;
  _updateApointer(fullCycle - 1, shift);
  sequence[apoint].motor1 = m1 - _liftm;
  sequence[apoint].motor2 = m2 - _liftm;
}

// create walk sequence
void _updateSequenceWalk(unsigned char center, char m1[], char m2[], motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1[center] - _liftm - _liftm / 2;
  sequence[apoint].motor2 = m2[center] - _liftm - _liftm / 2;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1[center - 5] - _liftm - _liftm;
  sequence[apoint].motor2 = m2[center - 5] - _liftm / 2;
  _updateApointer(2, shift);
  sequence[apoint].motor1 = m1[center - 7] - _liftm - _liftm;
  sequence[apoint].motor2 = m2[center - 7];
  _updateApointer(3, shift);
  sequence[apoint].motor1 = m1[center - 6];
  sequence[apoint].motor2 = m2[center - 6];
  _updateApointer(4, shift);
  sequence[apoint].motor1 = m1[center - 5];
  sequence[apoint].motor2 = m2[center - 5];
  _updateApointer(5, shift);
  sequence[apoint].motor1 = m1[center - 4];
  sequence[apoint].motor2 = m2[center - 4];
  _updateApointer(6, shift);
  sequence[apoint].motor1 = m1[center - 3];
  sequence[apoint].motor2 = m2[center - 3];
  _updateApointer(7, shift);
  sequence[apoint].motor1 = m1[center - 2];
  sequence[apoint].motor2 = m2[center - 2];
  _updateApointer(8, shift);
  sequence[apoint].motor1 = m1[center - 1];
  sequence[apoint].motor2 = m2[center - 1];
  _updateApointer(9, shift);
  sequence[apoint].motor1 = m1[center];
  sequence[apoint].motor2 = m2[center];
  _updateApointer(10, shift);
  sequence[apoint].motor1 = m1[center + 1];
  sequence[apoint].motor2 = m2[center + 1];
  _updateApointer(11, shift);
  sequence[apoint].motor1 = m1[center + 2];
  sequence[apoint].motor2 = m2[center + 2];
  _updateApointer(12, shift);
  sequence[apoint].motor1 = m1[center + 3];
  sequence[apoint].motor2 = m2[center + 3];
  _updateApointer(13, shift);
  sequence[apoint].motor1 = m1[center + 4];
  sequence[apoint].motor2 = m2[center + 4];
  _updateApointer(14, shift);
  sequence[apoint].motor1 = m1[center + 5];
  sequence[apoint].motor2 = m2[center + 5];
  _updateApointer(15, shift);
  sequence[apoint].motor1 = m1[center + 6];
  sequence[apoint].motor2 = m2[center + 6];
  _updateApointer(16, shift);
  sequence[apoint].motor1 = m1[center + 7];
  sequence[apoint].motor2 = m2[center + 7] - _liftm / 2;
  _updateApointer(17, shift);
  sequence[apoint].motor1 = m1[center + 4] - _liftm / 2;
  sequence[apoint].motor2 = m2[center + 4] - _liftm;
}

// create slow walk sequence
void _updateSequenceWalkSlow(unsigned char center, char m1[], char m2[], motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1[center] - _liftm - _liftm / 2;
  sequence[apoint].motor2 = m2[center] - _liftm - _liftm / 2;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1[center - 4] - _liftm - _liftm;
  sequence[apoint].motor2 = m2[center - 4] - _liftm / 2;
  _updateApointer(2, shift);
  sequence[apoint].motor1 = m1[center - 4] - _liftm - _liftm;
  sequence[apoint].motor2 = m2[center - 4];
  _updateApointer(3, shift);
  sequence[apoint].motor1 = m1[center - 3];
  sequence[apoint].motor2 = m2[center - 3];
  _updateApointer(4, shift);
  sequence[apoint].motor1 = m1[center - 3];
  sequence[apoint].motor2 = m2[center - 3];
  _updateApointer(5, shift);
  sequence[apoint].motor1 = m1[center - 2];
  sequence[apoint].motor2 = m2[center - 2];
  _updateApointer(6, shift);
  sequence[apoint].motor1 = m1[center - 2];
  sequence[apoint].motor2 = m2[center - 2];
  _updateApointer(7, shift);
  sequence[apoint].motor1 = m1[center - 1];
  sequence[apoint].motor2 = m2[center - 1];
  _updateApointer(8, shift);
  sequence[apoint].motor1 = m1[center - 1];
  sequence[apoint].motor2 = m2[center - 1];
  _updateApointer(9, shift);
  sequence[apoint].motor1 = m1[center];
  sequence[apoint].motor2 = m2[center];
  _updateApointer(10, shift);
  sequence[apoint].motor1 = m1[center];
  sequence[apoint].motor2 = m2[center];
  _updateApointer(11, shift);
  sequence[apoint].motor1 = m1[center + 1];
  sequence[apoint].motor2 = m2[center + 1];
  _updateApointer(12, shift);
  sequence[apoint].motor1 = m1[center + 1];
  sequence[apoint].motor2 = m2[center + 1];
  _updateApointer(13, shift);
  sequence[apoint].motor1 = m1[center + 2];
  sequence[apoint].motor2 = m2[center + 2];
  _updateApointer(14, shift);
  sequence[apoint].motor1 = m1[center + 2];
  sequence[apoint].motor2 = m2[center + 2];
  _updateApointer(15, shift);
  sequence[apoint].motor1 = m1[center + 3];
  sequence[apoint].motor2 = m2[center + 3];
  _updateApointer(16, shift);
  sequence[apoint].motor1 = m1[center + 3];
  sequence[apoint].motor2 = m2[center + 3] - _liftm / 2;
  _updateApointer(17, shift);
  sequence[apoint].motor1 = m1[center + 4] - _liftm / 2;
  sequence[apoint].motor2 = m2[center + 4] - _liftm;
}

// create walk back sequence
void _updateSequenceWalkBack(unsigned char center, char m1[], char m2[], motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1[center] - _liftm - _liftm / 2;
  sequence[apoint].motor2 = m2[center] - _liftm - _liftm / 2;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1[center + 4] - _liftm - _liftm;
  sequence[apoint].motor2 = m2[center + 4] - _liftm / 2;
  _updateApointer(2, shift);
  sequence[apoint].motor1 = m1[center + 4] - _liftm - _liftm;
  sequence[apoint].motor2 = m2[center + 4];
  _updateApointer(3, shift);
  sequence[apoint].motor1 = m1[center + 3];
  sequence[apoint].motor2 = m2[center + 3];
  _updateApointer(4, shift);
  sequence[apoint].motor1 = m1[center + 3];
  sequence[apoint].motor2 = m2[center + 3];
  _updateApointer(5, shift);
  sequence[apoint].motor1 = m1[center + 2];
  sequence[apoint].motor2 = m2[center + 2];
  _updateApointer(6, shift);
  sequence[apoint].motor1 = m1[center + 2];
  sequence[apoint].motor2 = m2[center + 2];
  _updateApointer(7, shift);
  sequence[apoint].motor1 = m1[center + 1];
  sequence[apoint].motor2 = m2[center + 1];
  _updateApointer(8, shift);
  sequence[apoint].motor1 = m1[center + 1];
  sequence[apoint].motor2 = m2[center + 1];
  _updateApointer(9, shift);
  sequence[apoint].motor1 = m1[center];
  sequence[apoint].motor2 = m2[center];
  _updateApointer(10, shift);
  sequence[apoint].motor1 = m1[center];
  sequence[apoint].motor2 = m2[center];
  _updateApointer(11, shift);
  sequence[apoint].motor1 = m1[center - 1];
  sequence[apoint].motor2 = m2[center - 1];
  _updateApointer(12, shift);
  sequence[apoint].motor1 = m1[center - 1];
  sequence[apoint].motor2 = m2[center - 1];
  _updateApointer(13, shift);
  sequence[apoint].motor1 = m1[center - 2];
  sequence[apoint].motor2 = m2[center - 2];
  _updateApointer(14, shift);
  sequence[apoint].motor1 = m1[center - 2];
  sequence[apoint].motor2 = m2[center - 2];
  _updateApointer(15, shift);
  sequence[apoint].motor1 = m1[center - 3];
  sequence[apoint].motor2 = m2[center - 3];
  _updateApointer(16, shift);
  sequence[apoint].motor1 = m1[center - 3];
  sequence[apoint].motor2 = m2[center - 3] - _liftm / 2;
  _updateApointer(17, shift);
  sequence[apoint].motor1 = m1[center - 4] - _liftm / 2;
  sequence[apoint].motor2 = m2[center - 4] - _liftm;
} 


// create walk sequence
void _setSequenceRecover(motors * sequence1, motors * sequence2) {
  // leg 1
  sequence1[0].motor1 = 30; sequence1[0].motor2 = 30;
  sequence1[1].motor1 = 0; sequence1[1].motor2 = 0;
  for (i = 2; i < 16; i++) {
    sequence1[i].motor1 = -40; sequence1[i].motor2 = -40;
  }
  sequence1[16].motor1 = 0; sequence1[16].motor2 = 0;
  sequence1[17].motor1 = 30; sequence1[17].motor2 = 30;
  // leg 2
  sequence2[0].motor1 = 30; sequence2[0].motor2 = 30;
  sequence2[1].motor1 = 15; sequence2[1].motor2 = 15;
  sequence2[2].motor1 = 0; sequence2[2].motor2 = 0;
  sequence2[3].motor1 = 0; sequence2[3].motor2 = 0;
  sequence2[4].motor1 = 0; sequence2[4].motor2 = 0;
  sequence2[5].motor1 = 0; sequence2[5].motor2 = 0;
  for (i = 6; i < 16; i++) {
    sequence2[i].motor1 = 90; sequence2[i].motor2 = 90;
  }
  sequence2[16].motor1 = 60; sequence2[16].motor2 = 60;
  sequence2[17].motor1 = 30; sequence2[17].motor2 = 30;
}

// get next sequence
void setPattern(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSL[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSR[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFL[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFR[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRL[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRR[0]);
    }
    break;
    case P_STANDTOGO:
    {
      _updateSequenceLinearEnd(motor1Walk[_centerStatic] - _liftm, motor2Walk[_centerStatic] - _liftm, & sequenceSL[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSR[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFL[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic - 2] - _liftm, motor2Walk[_centerStatic - 2] - _liftm, & sequenceFR[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRL[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic + 2] - _liftm, motor2Walk[_centerStatic + 2] - _liftm, & sequenceRR[0]);
    }
    break;
    case P_GOTOSTAND:
    {
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSL[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSR[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFL[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFR[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRL[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRR[0]);
    }
    break;
    case P_STANDGO:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSL[0], 2);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSR[0], 11);
      _updateSequenceStandWalk(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFL[0], 13);
      _updateSequenceStandWalk(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFR[0], 4);
      _updateSequenceStandWalk(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRL[0], 9);
      _updateSequenceStandWalk(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRR[0], 0);
    }
    break;
    case P_STANDGOLEFT:
    {
      _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceWalkBack(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceWalkSlow(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceWalkBack(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceWalkSlow(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_STANDGORIGHT:
    {
      _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceWalkSlow(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceWalkBack(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceWalkSlow(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceWalkBack(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_GOFORWARD:
    {
      _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceWalk(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceWalk(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceWalk(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceWalk(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_GOFORWARDSLOW:
    {
      _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceWalkSlow(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceWalkSlow(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceWalkSlow(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceWalkSlow(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_GOLEFT:
    {
      _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceWalkSlow(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceWalk(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceWalkSlow(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceWalk(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_GORIGHT:
    {
      _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceWalk(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceWalkSlow(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceWalk(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceWalkSlow(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_GOBACK:
    {
      _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceWalkBack(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceWalkBack(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceWalkBack(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceWalkBack(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_GOBACKLEFT:
    {
      _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceSL[0], 2);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSR[0], 11);
      _updateSequenceWalkBack(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFL[0], 13);
      _updateSequenceStandWalk(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFR[0], 4);
      _updateSequenceWalkBack(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRL[0], 9);
      _updateSequenceStandWalk(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRR[0], 0);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceSL[0], 2);
      _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceSR[0], 11);
      _updateSequenceStandWalk(motor1Walk[_centerStatic - 2], motor2Walk[_centerStatic - 2], & sequenceFL[0], 13);
      _updateSequenceWalkBack(_centerStatic - 2, motor1Walk, motor2Walk, & sequenceFR[0], 4);
      _updateSequenceStandWalk(motor1Walk[_centerStatic + 2], motor2Walk[_centerStatic + 2], & sequenceRL[0], 9);
      _updateSequenceWalkBack(_centerStatic + 2, motor1Walk, motor2Walk, & sequenceRR[0], 0);
    }
    break;
    case P_DODOWN:
    {
      _updateSequenceLinear(30, 30, & sequenceSL[0]);
      _updateSequenceLinear(30, 30, & sequenceSR[0]);
      _updateSequenceLinear(30, 30, & sequenceFL[0]);
      _updateSequenceLinear(30, 30, & sequenceFR[0]);
      _updateSequenceLinear(30, 30, & sequenceRL[0]);
      _updateSequenceLinear(30, 30, & sequenceRR[0]);
    }
    break;
    case P_DODOWNLEFT:
    {
      _updateSequenceLinear(30, 30, & sequenceSL[0]);
      _updateSequenceLinear(45, 45, & sequenceSR[0]);
      _updateSequenceLinear(30, 30, & sequenceFL[0]);
      _updateSequenceLinear(45, 45, & sequenceFR[0]);
      _updateSequenceLinear(30, 30, & sequenceRL[0]);
      _updateSequenceLinear(45, 45, & sequenceRR[0]);
    }
    break;
    case P_DODOWNRIGHT:
    {
      _updateSequenceLinear(45, 45, & sequenceSL[0]);
      _updateSequenceLinear(30, 30, & sequenceSR[0]);
      _updateSequenceLinear(45, 45, & sequenceFL[0]);
      _updateSequenceLinear(30, 30, & sequenceFR[0]);
      _updateSequenceLinear(45, 45, & sequenceRL[0]);
      _updateSequenceLinear(30, 30, & sequenceRR[0]);
    }
    break;
    case P_DODOWNFRONT:
    {
      _updateSequenceLinear(40, 40, & sequenceSL[0]);
      _updateSequenceLinear(40, 40, & sequenceSR[0]);
      _updateSequenceLinear(30, 30, & sequenceFL[0]);
      _updateSequenceLinear(30, 30, & sequenceFR[0]);
      _updateSequenceLinear(50, 50, & sequenceRL[0]);
      _updateSequenceLinear(50, 50, & sequenceRR[0]);
    }
    break;
    case P_DODOWNREAR:
    {
      _updateSequenceLinear(40, 40, & sequenceSL[0]);
      _updateSequenceLinear(40, 40, & sequenceSR[0]);
      _updateSequenceLinear(50, 50, & sequenceFL[0]);
      _updateSequenceLinear(50, 50, & sequenceFR[0]);
      _updateSequenceLinear(30, 30, & sequenceRL[0]);
      _updateSequenceLinear(30, 30, & sequenceRR[0]);
    }
    break;
    case P_RECOVERLEFT:
    {
      _setSequenceRecover(& sequenceSL[0], & sequenceSR[0]);
      _setSequenceRecover(& sequenceFL[0], & sequenceFR[0]);
      _setSequenceRecover(& sequenceRL[0], & sequenceRR[0]);
    }
    break;
    case P_RECOVERRIGHT:
    {
      _setSequenceRecover(& sequenceSR[0], & sequenceSL[0]);
      _setSequenceRecover(& sequenceFR[0], & sequenceFL[0]);
      _setSequenceRecover(& sequenceRR[0], & sequenceRL[0]);
    }
    break;
    default:
    break;
  }
}

// limit angle value
char limitValue(char mAngle) {
  if (mAngle > 90) {
    mAngle = 90;
  } else if (mAngle < -90) {
    mAngle = -90;
  }
  return mAngle;
}

// update servo motors buffer values
void updateBufferPatterns(allMotors calibration) {
  motorValueBuffer.m.fl.motor1 = limitValue(sequenceFL[mainCounter].motor1 - 30 + calibration.m.fl.motor1 + legUp.m.fl.motor1);
  motorValueBuffer.m.fl.motor2 = limitValue(sequenceFL[mainCounter].motor2 - 30 + calibration.m.fl.motor2 + legUp.m.fl.motor2);
  motorValueBuffer.m.fr.motor1 = limitValue(sequenceFR[mainCounter].motor1 - 30 + calibration.m.fr.motor1 + legUp.m.fr.motor1);
  motorValueBuffer.m.fr.motor2 = limitValue(sequenceFR[mainCounter].motor2 - 30 + calibration.m.fr.motor2 + legUp.m.fr.motor2);
  motorValueBuffer.m.sl.motor1 = limitValue(sequenceSL[mainCounter].motor1 - 30 + calibration.m.sl.motor1 + legUp.m.sl.motor1);
  motorValueBuffer.m.sl.motor2 = limitValue(sequenceSL[mainCounter].motor2 - 30 + calibration.m.sl.motor2 + legUp.m.sl.motor2);
  motorValueBuffer.m.sr.motor1 = limitValue(sequenceSR[mainCounter].motor1 - 30 + calibration.m.sr.motor1 + legUp.m.sr.motor1);
  motorValueBuffer.m.sr.motor2 = limitValue(sequenceSR[mainCounter].motor2 - 30 + calibration.m.sr.motor2 + legUp.m.sr.motor2);
  motorValueBuffer.m.rl.motor1 = limitValue(sequenceRL[mainCounter].motor1 - 30 + calibration.m.rl.motor1 + legUp.m.rl.motor1);
  motorValueBuffer.m.rl.motor2 = limitValue(sequenceRL[mainCounter].motor2 - 30 + calibration.m.rl.motor2 + legUp.m.rl.motor2);
  motorValueBuffer.m.rr.motor1 = limitValue(sequenceRR[mainCounter].motor1 - 30 + calibration.m.rr.motor1 + legUp.m.rr.motor1);
  motorValueBuffer.m.rr.motor2 = limitValue(sequenceRR[mainCounter].motor2 - 30 + calibration.m.rr.motor2 + legUp.m.rr.motor2);
}

// update servo motors buffer old values
void updateBufferOldPatterns(void) {
  motorValueBufferOld.m.fl.motor1 = motorValueBuffer.m.fl.motor1;
  motorValueBufferOld.m.fl.motor2 = motorValueBuffer.m.fl.motor2;
  motorValueBufferOld.m.fr.motor1 = motorValueBuffer.m.fr.motor1;
  motorValueBufferOld.m.fr.motor2 = motorValueBuffer.m.fr.motor2;
  motorValueBufferOld.m.sl.motor1 = motorValueBuffer.m.sl.motor1;
  motorValueBufferOld.m.sl.motor2 = motorValueBuffer.m.sl.motor2;
  motorValueBufferOld.m.sr.motor1 = motorValueBuffer.m.sr.motor1;
  motorValueBufferOld.m.sr.motor2 = motorValueBuffer.m.sr.motor2;
  motorValueBufferOld.m.rl.motor1 = motorValueBuffer.m.rl.motor1;
  motorValueBufferOld.m.rl.motor2 = motorValueBuffer.m.rl.motor2;
  motorValueBufferOld.m.rr.motor1 = motorValueBuffer.m.rr.motor1;
  motorValueBufferOld.m.rr.motor2 = motorValueBuffer.m.rr.motor2;
}

// update servo motors values
allMotors updateMotorsQuarter1Patterns(void) {
  motorValue.m.fl.motor1 = (motorValueBuffer.m.fl.motor1 + motorValueBufferOld.m.fl.motor1 * 3) / 4;
  motorValue.m.fl.motor2 = (motorValueBuffer.m.fl.motor2 + motorValueBufferOld.m.fl.motor2 * 3) / 4;
  motorValue.m.fr.motor1 = (motorValueBuffer.m.fr.motor1 + motorValueBufferOld.m.fr.motor1 * 3) / 4;
  motorValue.m.fr.motor2 = (motorValueBuffer.m.fr.motor2 + motorValueBufferOld.m.fr.motor2 * 3) / 4;
  motorValue.m.sl.motor1 = (motorValueBuffer.m.sl.motor1 + motorValueBufferOld.m.sl.motor1 * 3) / 4;
  motorValue.m.sl.motor2 = (motorValueBuffer.m.sl.motor2 + motorValueBufferOld.m.sl.motor2 * 3) / 4;
  motorValue.m.sr.motor1 = (motorValueBuffer.m.sr.motor1 + motorValueBufferOld.m.sr.motor1 * 3) / 4;
  motorValue.m.sr.motor2 = (motorValueBuffer.m.sr.motor2 + motorValueBufferOld.m.sr.motor2 * 3) / 4;
  motorValue.m.rl.motor1 = (motorValueBuffer.m.rl.motor1 + motorValueBufferOld.m.rl.motor1 * 3) / 4;
  motorValue.m.rl.motor2 = (motorValueBuffer.m.rl.motor2 + motorValueBufferOld.m.rl.motor2 * 3) / 4;
  motorValue.m.rr.motor1 = (motorValueBuffer.m.rr.motor1 + motorValueBufferOld.m.rr.motor1 * 3) / 4;
  motorValue.m.rr.motor2 = (motorValueBuffer.m.rr.motor2 + motorValueBufferOld.m.rr.motor2 * 3) / 4;
  return motorValue;
}

// update servo motors values
allMotors updateMotorsHalfPatterns(void) {
  motorValue.m.fl.motor1 = (motorValueBuffer.m.fl.motor1 + motorValueBufferOld.m.fl.motor1) / 2;
  motorValue.m.fl.motor2 = (motorValueBuffer.m.fl.motor2 + motorValueBufferOld.m.fl.motor2) / 2;
  motorValue.m.fr.motor1 = (motorValueBuffer.m.fr.motor1 + motorValueBufferOld.m.fr.motor1) / 2;
  motorValue.m.fr.motor2 = (motorValueBuffer.m.fr.motor2 + motorValueBufferOld.m.fr.motor2) / 2;
  motorValue.m.sl.motor1 = (motorValueBuffer.m.sl.motor1 + motorValueBufferOld.m.sl.motor1) / 2;
  motorValue.m.sl.motor2 = (motorValueBuffer.m.sl.motor2 + motorValueBufferOld.m.sl.motor2) / 2;
  motorValue.m.sr.motor1 = (motorValueBuffer.m.sr.motor1 + motorValueBufferOld.m.sr.motor1) / 2;
  motorValue.m.sr.motor2 = (motorValueBuffer.m.sr.motor2 + motorValueBufferOld.m.sr.motor2) / 2;
  motorValue.m.rl.motor1 = (motorValueBuffer.m.rl.motor1 + motorValueBufferOld.m.rl.motor1) / 2;
  motorValue.m.rl.motor2 = (motorValueBuffer.m.rl.motor2 + motorValueBufferOld.m.rl.motor2) / 2;
  motorValue.m.rr.motor1 = (motorValueBuffer.m.rr.motor1 + motorValueBufferOld.m.rr.motor1) / 2;
  motorValue.m.rr.motor2 = (motorValueBuffer.m.rr.motor2 + motorValueBufferOld.m.rr.motor2) / 2;
  return motorValue;
}

// update servo motors values
allMotors updateMotorsQuarter3Patterns(void) {
  motorValue.m.fl.motor1 = (motorValueBuffer.m.fl.motor1 * 3 + motorValueBufferOld.m.fl.motor1) / 4;
  motorValue.m.fl.motor2 = (motorValueBuffer.m.fl.motor2 * 3 + motorValueBufferOld.m.fl.motor2) / 4;
  motorValue.m.fr.motor1 = (motorValueBuffer.m.fr.motor1 * 3 + motorValueBufferOld.m.fr.motor1) / 4;
  motorValue.m.fr.motor2 = (motorValueBuffer.m.fr.motor2 * 3 + motorValueBufferOld.m.fr.motor2) / 4;
  motorValue.m.sl.motor1 = (motorValueBuffer.m.sl.motor1 * 3 + motorValueBufferOld.m.sl.motor1) / 4;
  motorValue.m.sl.motor2 = (motorValueBuffer.m.sl.motor2 * 3 + motorValueBufferOld.m.sl.motor2) / 4;
  motorValue.m.sr.motor1 = (motorValueBuffer.m.sr.motor1 * 3 + motorValueBufferOld.m.sr.motor1) / 4;
  motorValue.m.sr.motor2 = (motorValueBuffer.m.sr.motor2 * 3 + motorValueBufferOld.m.sr.motor2) / 4;
  motorValue.m.rl.motor1 = (motorValueBuffer.m.rl.motor1 * 3 + motorValueBufferOld.m.rl.motor1) / 4;
  motorValue.m.rl.motor2 = (motorValueBuffer.m.rl.motor2 * 3 + motorValueBufferOld.m.rl.motor2) / 4;
  motorValue.m.rr.motor1 = (motorValueBuffer.m.rr.motor1 * 3 + motorValueBufferOld.m.rr.motor1) / 4;
  motorValue.m.rr.motor2 = (motorValueBuffer.m.rr.motor2 * 3 + motorValueBufferOld.m.rr.motor2) / 4;
  return motorValue;
}

// update servo motors values
allMotors updateMotorsPatterns(void) {
  // set motors angle values
  motorValue.m.fl.motor1 = motorValueBuffer.m.fl.motor1;
  motorValue.m.fl.motor2 = motorValueBuffer.m.fl.motor2;
  motorValue.m.fr.motor1 = motorValueBuffer.m.fr.motor1;
  motorValue.m.fr.motor2 = motorValueBuffer.m.fr.motor2;
  motorValue.m.sl.motor1 = motorValueBuffer.m.sl.motor1;
  motorValue.m.sl.motor2 = motorValueBuffer.m.sl.motor2;
  motorValue.m.sr.motor1 = motorValueBuffer.m.sr.motor1;
  motorValue.m.sr.motor2 = motorValueBuffer.m.sr.motor2;
  motorValue.m.rl.motor1 = motorValueBuffer.m.rl.motor1;
  motorValue.m.rl.motor2 = motorValueBuffer.m.rl.motor2;
  motorValue.m.rr.motor1 = motorValueBuffer.m.rr.motor1;
  motorValue.m.rr.motor2 = motorValueBuffer.m.rr.motor2;
  return motorValue;
}

void updateBallanceInPattern( int roll, int pitch) {
  if (roll > 1) {
    if (correctRoll < 10) {
      correctRoll ++;
    }
  } else if (roll < -1) {
    if (correctRoll > -10) {
      correctRoll --;
    }
  } //else if (correctRoll < 0) {
  //  correctRoll ++;
  //} else if (correctRoll > 0) {
  //  correctRoll --;
  //}
  if (pitch > 1) {
    if (correctPitch < 10) {
      correctPitch ++;
    }
  } else if (pitch < -1) {
    if (correctPitch > -10) {
      correctPitch --;
    }
  } //else if (correctPitch < 0) {
  //  correctPitch ++;
  //} else if (correctPitch > 0) {
  //  correctPitch --;
  //}
  legUp.m.fl.motor1 = -correctRoll + correctPitch;
  legUp.m.fl.motor2 = -correctRoll + correctPitch;
  legUp.m.fr.motor1 = correctRoll + correctPitch;
  legUp.m.fr.motor2 = correctRoll + correctPitch;
  legUp.m.sl.motor1 = -correctRoll;
  legUp.m.sl.motor2 = -correctRoll;
  legUp.m.sr.motor1 = correctRoll;
  legUp.m.sr.motor2 = correctRoll;
  legUp.m.rl.motor1 = -correctRoll - correctPitch;
  legUp.m.rl.motor2 = -correctRoll - correctPitch;
  legUp.m.rr.motor1 = correctRoll - correctPitch;
  legUp.m.rr.motor2 = correctRoll - correctPitch;
}

// update servo motors values
unsigned char updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= fullCycle) {
    mainCounter = 0;
  }
  return mainCounter;
}

/*
// print sequence name
void printPatternName(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      Serial.println(" P_DOSTAND ");
    }
    break;
    case P_STANDTOGO:
    {
      Serial.println(" P_STANDTOGO ");
    }
    break;
    case P_GOTOSTAND:
    {
      Serial.println(" P_GOTOSTAND ");
    }
    break;
    case P_STANDGO:
    {
      Serial.println(" P_STANDGO ");
    }
    break;
    case P_STANDGOLEFT:
    {
      Serial.println(" P_STANDGOLEFT ");
    }
    break;
    case P_STANDGORIGHT:
    {
      Serial.println(" P_STANDGORIGHT ");
    }
    break;
    case P_GOFORWARD:
    {
      Serial.println(" P_GOFORWARD ");
    }
    break;
    case P_GOFORWARDSLOW:
    {
      Serial.println(" P_GOFORWARDSLOW ");
    }
    break;
    case P_GOLEFT:
    {
      Serial.println(" P_GOLEFT ");
    }
    break;
    case P_GORIGHT:
    {
      Serial.println(" P_GORIGHT ");
    }
    break;
    case P_GOBACK:
    {
      Serial.println(" P_GOBACK ");
    }
    break;
    case P_GOBACKLEFT:
    {
      Serial.println(" P_GOBACKLEFT ");
    }
    break;
    case P_GOBACKRIGHT:
    {
      Serial.println(" P_GOBACKRIGHT ");
    }
    break;
    case P_DONE:
    {
      Serial.println(" P_DONE ");
    }
    break;
    case P_END:
    {
      Serial.println(" P_END ");
    }
    break;
    case P_RESETDIRECTION:
    {
      Serial.println(" P_RESETDIRECTION ");
    }
    case P_RESETGIRO:
    {
      Serial.println(" P_RESETGIRO ");
    }
    break;
    case P_RESTOREDIRECTION:
    {
      Serial.println(" P_RESTOREDIRECTION ");
    }
    break;
    case P_ENABLEINPUTS:
    {
      Serial.println(" P_ENABLEINPUTS ");
    }
    break;
    case P_DISABLEINPUTS:
    {
      Serial.println(" P_DISABLEINPUTS ");
    }
    break;
    case P_DODOWN:
    {
      Serial.println(" P_DODOWN ");
    }
    break;
    case P_DODOWNLEFT:
    {
      Serial.println(" P_DODOWNLEFT ");
    }
    break;
    case P_DODOWNRIGHT:
    {
      Serial.println(" P_DODOWNRIGHT ");
    }
    break;
    case P_DODOWNFRONT:
    {
      Serial.println(" P_DODOWNFRONT ");
    }
    break;
    case P_DODOWNREAR:
    {
      Serial.println(" P_DODOWNREAR ");
    }
    break;
    case P_RECOVERLEFT:
    {
      Serial.println(" P_RECOVERLEFT ");
    }
    break;
    case P_RECOVERRIGHT:
    {
      Serial.println(" P_RECOVERRIGHT ");
    }
    break;
    default:
      Serial.println(" unknown pattern ");
    break;
  }
}
*/
