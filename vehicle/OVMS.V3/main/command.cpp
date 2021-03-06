/*
;    Project:       Open Vehicle Monitor System
;    Date:          14th March 2017
;
;    Changes:
;    1.0  Initial release
;
;    (C) 2011       Michael Stegen / Stegen Electronics
;    (C) 2011-2017  Mark Webb-Johnson
;    (C) 2011        Sonny Chen @ EPRO/DX
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "command.h"

OvmsCommandApp MyCommandApp __attribute__ ((init_priority (1000)));

OvmsWriter::OvmsWriter()
  {
  }

OvmsWriter::~OvmsWriter()
  {
  }

int OvmsWriter::puts(const char* s)
  {
  return 0;
  }

int OvmsWriter::printf(const char* fmt, ...)
  {
  return 0;
  }

ssize_t OvmsWriter::write(const void *buf, size_t nbyte)
  {
  return 0;
  }

void OvmsWriter::finalise()
  {
  }

OvmsCommand::OvmsCommand()
  {
  }

OvmsCommand::OvmsCommand(std::string title, void (*execute)(int, OvmsWriter*, int, const char* const*))
  {
  m_title = title;
  m_execute = execute;
  }

OvmsCommand::~OvmsCommand()
  {
  }

std::string OvmsCommand::GetTitle()
  {
  return m_title;
  }

OvmsCommand* OvmsCommand::RegisterCommand(std::string name, std::string title, void (*execute)(int, OvmsWriter*, int, const char* const*))
  {
  OvmsCommand* cmd = new OvmsCommand(title,execute);
  m_children[name] = cmd;
  //printf("Registered '%s' under '%s'\n",name.c_str(),m_title.c_str());
  return cmd;
  }

void OvmsCommand::Execute(int verbosity, OvmsWriter* writer, int argc, const char * const * argv)
  {
//  if (argc>0)
//    {
//    printf("Execute(%s/%d) verbosity=%d argc=%d first=%s\n",m_title.c_str(), m_children.size(), verbosity,argc,argv[0]);
//    }
//  else
//    {
//    printf("Execute(%s/%d) verbosity=%d (no args)\n",m_title.c_str(), m_children.size(), verbosity);
//    }

  if (m_execute)
    {
    //puts("Executing directly...");
    m_execute(verbosity,writer,argc,argv);
    return;
    }
  else
    {
    //puts("Looking for a matching command");
    if (strcmp(argv[0],"?")==0)
      {
      // Show available commands
      for (std::map<std::string, OvmsCommand*>::iterator it=m_children.begin(); it!=m_children.end(); ++it)
        {
        const char* k = it->first.c_str();
        const char* v = it->second->GetTitle().c_str();
        writer->printf("%-20.20s %s\n",k,v);
        }
      return;
      }
    std::map<std::string, OvmsCommand*>::iterator it = m_children.find(argv[0]);
    if (it == m_children.end())
      {
      writer->puts("Error: Unrecognised command");
      return;
      }
    OvmsCommand* cmd = it->second;
    if (argc>1)
      {
      cmd->Execute(verbosity,writer,argc-1,++argv);
      }
    else
      {
      cmd->Execute(verbosity,writer,0,NULL);
      }
    }
  }

void help(int verbosity, OvmsWriter* writer, int argc, const char* const* argv)
    {
    writer->puts("This isn't really much help, is it?");
    }

OvmsCommandApp::OvmsCommandApp()
  {
  puts("Initialising COMMAND Framework");
  m_root.RegisterCommand("help","Ask for help",help);
  }

OvmsCommandApp::~OvmsCommandApp()
  {
  }

OvmsCommand* OvmsCommandApp::RegisterCommand(std::string name, std::string title, void (*execute)(int, OvmsWriter*, int, const char* const*))
  {
  return m_root.RegisterCommand(name,title,execute);
  }

void OvmsCommandApp::Execute(int verbosity, OvmsWriter* writer, int argc, const char * const * argv)
  {
  if (argc == 0)
    {
    writer->puts("Error: Empty command unrecognised");
    }
  else
    {
    m_root.Execute(verbosity, writer, argc, argv);
    }
  }
