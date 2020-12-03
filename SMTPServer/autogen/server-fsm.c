/*   -*- buffer-read-only: t -*- vi: set ro:
 *
 *  DO NOT EDIT THIS FILE   (server-fsm.c)
 *
 *  It has been AutoGen-ed
 *  From the definitions    server.def
 *  and the template file   fsm
 *
 *  Automated Finite State Machine
 *
 *  Copyright (C) 1992-2018 Bruce Korb - all rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Bruce Korb'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * AutoFSM IS PROVIDED BY Bruce Korb ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Bruce Korb OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define DEFINE_FSM
#include "server-fsm.h"
#include <stdio.h>

/*
 *  Do not make changes to this file, except between the START/END
 *  comments, or it will be removed the next time it is generated.
 */
/* START === USER HEADERS === DO NOT CHANGE THIS COMMENT */
#include <stdlib.h>
/* END   === USER HEADERS === DO NOT CHANGE THIS COMMENT */

#ifndef NULL
#  define NULL 0
#endif

/**
 *  Enumeration of the valid transition types
 *  Some transition types may be common to several transitions.
 */
typedef enum {
    SERVER_FSM_TR_ACCEPTED,
    SERVER_FSM_TR_CLOSE,
    SERVER_FSM_TR_INVALID
} te_server_fsm_trans;
#define SERVER_FSM_TRANSITION_CT  3

/**
 *  State transition handling map.  Map the state enumeration and the event
 *  enumeration to the new state and the transition enumeration code (in that
 *  order).  It is indexed by first the current state and then the event code.
 */
typedef struct server_fsm_transition t_server_fsm_transition;
struct server_fsm_transition {
    te_server_fsm_state  next_state;
    te_server_fsm_trans  transition;
};
static const t_server_fsm_transition
server_fsm_trans_table[ SERVER_FSM_STATE_CT ][ SERVER_FSM_EVENT_CT ] = {

  /* STATE 0:  SERVER_FSM_ST_INIT */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID } /* EVT:  CONN_LOST */
  },


  /* STATE 1:  SERVER_FSM_ST_READY */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_CLOSED, SERVER_FSM_TR_CLOSE }   /* EVT:  CONN_LOST */
  },


  /* STATE 2:  SERVER_FSM_ST_NEED_SENDER */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID } /* EVT:  CONN_LOST */
  },


  /* STATE 3:  SERVER_FSM_ST_NEED_RECIPIENT */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID } /* EVT:  CONN_LOST */
  },


  /* STATE 4:  SERVER_FSM_ST_NEED_DATA */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID } /* EVT:  CONN_LOST */
  },


  /* STATE 5:  SERVER_FSM_ST_WAITING_FOR_DATA */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID } /* EVT:  CONN_LOST */
  },


  /* STATE 6:  SERVER_FSM_ST_SAVING_MAIL */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID } /* EVT:  CONN_LOST */
  },


  /* STATE 7:  SERVER_FSM_ST_CLOSED */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_NOOP */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_RSET */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  CMND_QUIT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_END */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  MAIL_SAVED */
    { SERVER_FSM_ST_READY, SERVER_FSM_TR_ACCEPTED }, /* EVT:  CONN_ACCEPTED */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID } /* EVT:  CONN_LOST */
  }
};


#define Server_FsmFsmErr_off     19
#define Server_FsmEvInvalid_off  75
#define Server_FsmStInit_off     83


static char const zServer_FsmStrings[311] =
/*     0 */ "** OUT-OF-RANGE **\0"
/*    19 */ "FSM Error:  in state %d (%s), event %d (%s) is invalid\n\0"
/*    75 */ "invalid\0"
/*    83 */ "init\0"
/*    88 */ "ready\0"
/*    94 */ "need_sender\0"
/*   106 */ "need_recipient\0"
/*   121 */ "need_data\0"
/*   131 */ "waiting_for_data\0"
/*   148 */ "saving_mail\0"
/*   160 */ "closed\0"
/*   167 */ "cmnd_noop\0"
/*   177 */ "cmnd_helo\0"
/*   187 */ "cmnd_ehlo\0"
/*   197 */ "cmnd_mail\0"
/*   207 */ "cmnd_rcpt\0"
/*   217 */ "cmnd_vrfy\0"
/*   227 */ "cmnd_data\0"
/*   237 */ "cmnd_rset\0"
/*   247 */ "cmnd_quit\0"
/*   257 */ "mail_end\0"
/*   266 */ "mail_data\0"
/*   276 */ "mail_saved\0"
/*   287 */ "conn_accepted\0"
/*   301 */ "conn_lost";

static const size_t aszServer_FsmStates[8] = {
    83,  88,  94,  106, 121, 131, 148, 160 };

static const size_t aszServer_FsmEvents[15] = {
    167, 177, 187, 197, 207, 217, 227, 237, 247, 257, 266, 276, 287, 301, 75 };


#define SERVER_FSM_EVT_NAME(t)   ( (((unsigned)(t)) >= 15) \
    ? zServer_FsmStrings : zServer_FsmStrings + aszServer_FsmEvents[t])

#define SERVER_FSM_STATE_NAME(s) ( (((unsigned)(s)) >= 8) \
    ? zServer_FsmStrings : zServer_FsmStrings + aszServer_FsmStates[s])

#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

static int server_fsm_invalid_transition( te_server_fsm_state st, te_server_fsm_event evt );

/* * * * * * * * * THE CODE STARTS HERE * * * * * * * */
/**
 *  Print out an invalid transition message and return EXIT_FAILURE
 */
static int
server_fsm_invalid_transition( te_server_fsm_state st, te_server_fsm_event evt )
{
    /* START == INVALID TRANS MSG == DO NOT CHANGE THIS COMMENT */
    char const * fmt = zServer_FsmStrings + Server_FsmFsmErr_off;
    fprintf( stderr, fmt, st, SERVER_FSM_STATE_NAME(st), evt, SERVER_FSM_EVT_NAME(evt));
    /* END   == INVALID TRANS MSG == DO NOT CHANGE THIS COMMENT */

    return EXIT_FAILURE;
}

/**
 *  Step the FSM.  Returns the resulting state.  If the current state is
 *  SERVER_FSM_ST_DONE or SERVER_FSM_ST_INVALID, it resets to
 *  SERVER_FSM_ST_INIT and returns SERVER_FSM_ST_INIT.
 */
te_server_fsm_state
server_fsm_step(
    te_server_fsm_state server_fsm_state,
    te_server_fsm_event trans_evt,
    int client_socket_fd,
    char*** matchdata,
    int matchdatalen,
    int** matchdatasizes )
{
    te_server_fsm_state nxtSt;
    te_server_fsm_trans trans;

    if ((unsigned)server_fsm_state >= SERVER_FSM_ST_INVALID) {
        return SERVER_FSM_ST_INIT;
    }

#ifndef __COVERITY__
    if (trans_evt >= SERVER_FSM_EV_INVALID) {
        nxtSt = SERVER_FSM_ST_INVALID;
        trans = SERVER_FSM_TR_INVALID;
    } else
#endif /* __COVERITY__ */
    {
        const t_server_fsm_transition * ttbl =
            server_fsm_trans_table[ server_fsm_state ] + trans_evt;
        nxtSt = ttbl->next_state;
        trans = ttbl->transition;
    }


    switch (trans) {
    case SERVER_FSM_TR_ACCEPTED:
        /* START == ACCEPTED == DO NOT CHANGE THIS COMMENT */
        //nxtSt = HANDLE_ACCEPTED();
        /* END   == ACCEPTED == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_CLOSE:
        /* START == CLOSE == DO NOT CHANGE THIS COMMENT */
        //nxtSt = HANDLE_CLOSE();
        /* END   == CLOSE == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INVALID:
        /* START == INVALID == DO NOT CHANGE THIS COMMENT */
        exit(server_fsm_invalid_transition(server_fsm_state, trans_evt));
        /* END   == INVALID == DO NOT CHANGE THIS COMMENT */
        break;


    default:
        /* START == BROKEN MACHINE == DO NOT CHANGE THIS COMMENT */
        exit(server_fsm_invalid_transition(server_fsm_state, trans_evt));
        /* END   == BROKEN MACHINE == DO NOT CHANGE THIS COMMENT */
    }


    /* START == FINISH STEP == DO NOT CHANGE THIS COMMENT */
    /* END   == FINISH STEP == DO NOT CHANGE THIS COMMENT */

    return nxtSt;
}
/*
 * Local Variables:
 * mode: C
 * c-file-style: "stroustrup"
 * indent-tabs-mode: nil
 * End:
 * end of server-fsm.c */
