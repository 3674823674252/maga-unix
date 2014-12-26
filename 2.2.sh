#!/bin/bash

set -e

function warn_about_long_runners()
{
  PROMPT_COMMAND_RUN_TIME=
  if [ "$PROMPT_COMMAND_LAST_DATE_PRETTY" ] && [ "$PROMPT_COMMAND_LAST_TS" ]
  then
    local ts_now=`date +%s`
    local pretty_now=`date +%H:%M:%S`
    local ts_diff=$(( $ts_now - $PROMPT_COMMAND_LAST_TS ))
    if [ $ts_diff -ne 0 ]
    then
      local red_color="\033[1;31m"
      local  no_color="\033[0m"
      if [ $ts_diff -ge 3600 ]
      then
        local hours=$(( $ts_diff / 3600 ))
        PROMPT_COMMAND_RUN_TIME="$hours hour(s), "
        ts_diff=$(( $ts_diff - $hours * 3600 ))
      fi
      if [ $ts_diff -ge 60 ]
      then
        local mins=$(( $ts_diff / 60 ))
        PROMPT_COMMAND_RUN_TIME="$PROMPT_COMMAND_RUN_TIME$mins minute(s), "
        ts_diff=$(( $ts_diff - $mins * 60 ))
      fi
      PROMPT_COMMAND_RUN_TIME="$PROMPT_COMMAND_RUN_TIME$ts_diff second(s)"
    fi
  fi

  if [ -n "$PROMPT_COMMAND_RUN_TIME" ]
  then 
    echo -e "$red_color[$PROMPT_COMMAND_LAST_DATE_PRETTY - $pretty_now ($PROMPT_COMMAND_RUN_TIME)]$no_color"
    PROMPT_COMMAND_LAST_TS=$ts_now
    PROMPT_COMMAND_LAST_DATE_PRETTY=$pretty_now
  fi
}

function preexec_hook() {
  if [ "$BASH_COMMAND" = "$PROMPT_COMMAND" ]
  then
    return
  else
    PROMPT_COMMAND_LAST_TS=`date +%s`
    PROMPT_COMMAND_LAST_DATE_PRETTY=`date +%H:%M:%S`
  fi
}

PROMPT_COMMAND="warn_about_long_runners"
trap 'preexec_hook' DEBUG