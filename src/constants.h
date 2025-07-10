#ifndef CONSTANTS_H
#define CONSTANTS_H

// ThingsBoard attribute keys
#define TB_CUP_SIZE_ATTR "cupSize"
#define TB_ML_PER_PULSE_ATTR "mlPerPulse"

// ThingsBoard RPC commands
#define TB_SET_CUP_SIZE_RPC "setCupSize"
#define TB_SET_ML_PER_PULSE_RPC "setMlPerPulse"
#define TB_STOP_POUR_RPC "stopPour"

// Hardware pins
#define RELAY_PIN 13
#define FLOW_SENSOR_PIN 27

// LED status indicator pin
#define LED_SYSTEM_PIN 2      // Single system status LED

// Flow sensor constants
#define DEFAULT_ML_PER_PULSE 2.222 // 450 pulses/liter ≈ 2.222ml/pulse

// Safety and monitoring constants
#define MAX_POUR_TIME 90000    // Maximum pour time in milliseconds (90 seconds)
#define MAX_POUR_VOLUME 2000   // Maximum pour volume in ml (2 liters)
#define MIN_CUP_SIZE 50        // Minimum cup size in ml
#define MAX_CUP_SIZE 2000      // Maximum cup size in ml
#define MIN_ML_PER_PULSE 0.5   // Minimum ml per pulse
#define MAX_ML_PER_PULSE 10.0  // Maximum ml per pulse
#define WATCHDOG_TIMEOUT 30000 // Watchdog timeout in milliseconds (30 seconds)


// Safety limits
#define MAX_PULSE_COUNT 1000000 // Sanity check for pulse count
#define MAX_VOLUME_SANITY 10000 // 10L sanity check for volume calculations

#endif // CONSTANTS_H
