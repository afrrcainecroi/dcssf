#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <thread>

//il vr_language
#include "va_scanner.h"
#include "va_parser.hpp"
#include "interpreter.h"
#include "command.h"

#include <mtfafs.h>
#include <mtfa_fs3.h>
#include <mtfa_utils.h>
#include <mtfa_db.h>

#include "rulemgr.h"

#include "VAengine.h"

using namespace VaParser;
using namespace std;

//SimpleRng * rng;
//extern ULL_Rng * rng;

bool DoCompileRules ( string LbUser_VA_VRules, string LbUser_VA_VReports, VAengine & vaeng )
{
     unsigned long long start, end;

     Interpreter i;
     i.clear();

     int res = 0;
     std::filebuf fb;
     if ( fb.open ( LbUser_VA_VRules, std::ios::in ) ) {
          std::istream is ( &fb );
          i.switchInputStream ( &is );
          res = i.parse();
          fb.close();
     }

     cout << __func__   << " - Parse complete. Result error = " << res << endl;
     if ( res != 0 ) {
          return false;
     }

     //Qui crea l'engine con i nuovi valori
     vaeng.validation_report_directory_name=LbUser_VA_VReports;
     AstParse ( vaeng, i );

     thread t_PurgeAddedAndRemovedValues ( VASets::PurgeAddedAndRemovedValues, &vaeng.vasets );
     t_PurgeAddedAndRemovedValues.detach();
     
     thread t_PurgeTracedValues ( VAengine::PurgeTracedValues, &vaeng.trace_ds);
     t_PurgeTracedValues.detach();

     //Semplifico le regole di accesso
     vaeng.RebuildARDataSet();

     //Ora posso provare a fare qualche semplificazione
     //Stampo le vr e poi le semplifico e le stampo semplificate
     for ( auto it: vaeng.varules ) {
          it.second.root = it.second.root->SimplifyNodeTree ( it.second.root );
          cout << __func__   << "\n\n\nVR: " << it.first << endl;
          it.second.root->Print ( 0, it.second.root );
          for ( auto jt: it.second.root->actions ) {
               cout << __func__   << " - T: " << VA_ActionTypesToString ( jt.atype )  << ". ";
               cout << __func__   << " - OB: " << VA_ItemTypesToString ( jt.observable ) << ". " << "OB_IDX: " << jt.obs_index << ". " << jt.val1 << ", " << jt.val2 << ". " << jt.vrep.size() << endl;
          }
     }

     for ( auto it: vaeng.acrules ) {
          cout << __func__   << "\n\n\nAR: " << it.first << endl;
          for ( auto jt: it.second.basic_access_criteria ) {
               cout << __func__   << " - BAR: " << VA_ItemTypesToString ( jt.itype )  << ". " << jt.op1 << ". " << jt.op2 << endl;
          }
//        for (auto jt: it.second.extended_access_criteria)
//        {
//            cout << __func__   << "XAR: " << VA_ItemTypesToString(jt.itype)  << ". " << jt.op1 << ". " << jt.op2 << endl;
//        }
          for ( auto jt: it.second.access_operations ) {
               cout << __func__   << " - AOP: " << jt.default_value << ". " << jt.var_name << endl;
          }
     }

     for ( auto & it : vaeng.vasets.datasets ) {
         DO_VASET_LOCK(it.second->rmtx);
          cout << "Name: " << it.first << ". Values: ";
          cout << mtfa_enum_to_int ( it.second->stype ) << ", ";
          cout << it.second->sname << ", ";
//        cout << (it.second->mss ? it.second->mss->db.size() : 0) << ", ";
//        cout << it.second->vb->size() << ", ";
//        cout << it.second->uss->size() << ", ";
          cout << it.second->initial_values.size() << ", ";
          cout << it.second->initial_values_pairs.size() << ", ";
          cout << it.second->operative_data.size() << ", ";
          cout << it.second->pars.size() << endl;
     }

     cout << __func__   << " - Parse complete. Result ok." << endl;
     return true;
}

