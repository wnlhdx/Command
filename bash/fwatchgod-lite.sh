#!/data/data/com.termux/files/usr/bin/bash

PLATFORM=$(uname -a)
TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
LOG_DIR="$HOME/my-watchdog/logs"
mkdir -p "$LOG_DIR"

log() {
  echo "$TIMESTAMP $1" >> "$LOG_DIR/watchdog.log"
}

check_cpu() {
  CPU=$(top -bn1 | grep 'Cpu(s)' | awk '{print $2 + $4}')
  if (( $(echo "$CPU > 90.0" | bc -l) )); then
    log "[WARN] High CPU: $CPU%"
  fi
}

check_mem() {
  FREE=$(free | grep Mem | awk '{print $4}')
  if [ "$FREE" -lt 200000 ]; then
    log "[WARN] Low Mem: $FREE KB"
  fi
}

log "[INFO] Running watchdog check..."
check_cpu
check_mem

log "[INFO] Summary of warnings in past 24 hours:"
grep "\[WARN\]" "$LOG_DIR/watchdog.log" | tail -n 10 >> "$LOG_DIR/watchdog.log"