// Used for gmtools if you didnt figure that out already.
use vitals;

const CORE_SETTINGS_MIN_ATTRIBUTE  := 0;
const CORE_SETTINGS_MAX_ATTRIBUTE  := 60000;
const CORE_SETTINGS_MIN_VITAL      := 100;
const CORE_SETTINGS_MAX_VITAL      := 10000000;
const CORE_SETTINGS_MIN_REGEN_RATE := -30000;
const CORE_SETTINGS_MAX_REGEN_RATE := 30000;

Function DoRecalcVitals( mob )
  set_critical(1);
  if(mob.acctname)
    if(mob.ip)
      RecalcVitals(mob);
    endif
  else
    RecalcVitals(mob);
  endif
  set_critical(0);
endfunction

function GetStrength(who)
  return GetAttribute(who, ATTRIBUTEID_STRENGTH);
endfunction

function SetBaseSkillBaseValue(who, skillid, skill_tenth)
  if(skill_tenth < CORE_SETTINGS_MIN_ATTRIBUTE)
    skill_tenth := CORE_SETTINGS_MIN_ATTRIBUTE;
  elseif(skill_tenth > CORE_SETTINGS_MAX_ATTRIBUTE)
    skill_tenth := CORE_SETTINGS_MAX_ATTRIBUTE;
  endif
  return SetAttributeBaseValue(who, GetAttributeIdBySkillId(skillid), Cint(skill_tenth));
endfunction

function SetBaseStrength(who, strength)
  strength := Cint(strength * 10);
  if(strength < 10)
    strength := 10;
  elseif(strength > CORE_SETTINGS_MAX_ATTRIBUTE)
    strength := CORE_SETTINGS_MAX_ATTRIBUTE;
  endif
  var ret := SetAttributeBaseValue(who, ATTRIBUTEID_STRENGTH, strength);
  if(ret)
    DoRecalcVitals(who);
  endif
  return ret;
endfunction

function SetBaseIntelligence(who, intelligence)
  intelligence := Cint(intelligence * 10);
  if(intelligence < 10)
    intelligence := 10;
  elseif(intelligence > CORE_SETTINGS_MAX_ATTRIBUTE)
    intelligence := CORE_SETTINGS_MAX_ATTRIBUTE;
  endif
  var ret := SetAttributeBaseValue(who, ATTRIBUTEID_INTELLIGENCE, intelligence);
  if(ret)
    DoRecalcVitals(who);
  endif
  return ret;
endfunction

function SetBaseDexterity(who, dexterity)
  dexterity := Cint(dexterity * 10);
  if(dexterity < 10)
    dexterity := 10;
  elseif(dexterity > CORE_SETTINGS_MAX_ATTRIBUTE)
    dexterity := CORE_SETTINGS_MAX_ATTRIBUTE;
  endif
  var ret := SetAttributeBaseValue(who, ATTRIBUTEID_DEXTERITY, dexterity);
  if(ret)
    DoRecalcVitals(who);
  endif
  return ret;
endfunction

function SetMana(who, amount)
  SetVital(who, "Mana", cint(amount*100.0));
endfunction

function SetStamina(who, stamina)
  return SetVital(who, VITALID_STAMINA, Cint(stamina*100));
endfunction

function GetDexterity(who)
  return GetAttribute(who, ATTRIBUTEID_DEXTERITY);
endfunction

function GetHp(who)
  return Cint(GetVital(who, VITALID_LIFE)/ 100);
endfunction

function GetMaxHp(who)
  return Cint(GetVitalMaximumValue(who, VITALID_LIFE)/ 100);
endfunction

function GetMaxMana(who)
  return Cint(GetVitalMaximumValue(who, VITALID_MANA)/ 100);
endfunction

function GetMaxStamina(who)
  return Cint(GetVitalMaximumValue(who, VITALID_STAMINA)/ 100);
endfunction

function GetMana(who)
  return(GetVital(who, "Mana")/ 100);
endfunction

function GetStamina(who)
  return Cint(GetVital(who, VITALID_STAMINA)/ 100);
endfunction




