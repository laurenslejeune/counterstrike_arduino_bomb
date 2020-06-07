#define ARM_TIME 2000		// ms
#define DISARM_TIME 5000	// ms
#define BOMB_TIME_MINUTES 1
#define BOMB_TIMER BOMB_TIME_MINUTES * 10

#define TIMER_CLK 125 //Timer clock is 125Hz

// Stage 1 when REM_TIME > STAGE2_REM_TIME seconds.
#define STAGE2_REM_TIME 60 // Second stage when 1 minute remains
#define STAGE3_REM_TIME 30 // Third stage when 30 seconds remain
#define STAGE4_REM_TIME 10 // Second to last stage stage when 10 seconds remain
#define STAGE5_REM_TIME 5  // Last stage when 10 seconds remain

// Set frequency
#define STAGE1_FREQUENCY 0.5
#define STAGE2_FREQUENCY 1
#define STAGE3_FREQUENCY 2
#define STAGE4_FREQUENCY 4
#define STAGE5_FREQUENCY 10

#define STAGE1_MOD (int)(TIMER_CLK / STAGE1_FREQUENCY)
#define STAGE2_MOD (int)(TIMER_CLK / STAGE2_FREQUENCY)
#define STAGE3_MOD (int)(TIMER_CLK / STAGE3_FREQUENCY)
#define STAGE4_MOD (int)(TIMER_CLK / STAGE4_FREQUENCY)
#define STAGE5_MOD (int)(TIMER_CLK / STAGE5_FREQUENCY)

#define ARMING_MOD (int)(TIMER_CLK / 4)
enum states {
  INIT,
  ARMING,
  ARMED,
  DEFUSING,
  DEFUSED,
  EXPLODING,
  EXPLODED
};