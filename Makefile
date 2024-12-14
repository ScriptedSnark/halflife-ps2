EE_BIN_NAME = GearboxTrolling
EE_BIN = GearboxTrolling.elf

PS2_BUILD = 0
NEWLIB_NANO = 0
KERNEL_NOPATCH = 0

PS2_DIR=$(SOURCE_DIR)/ps2

# HL SDK
######################################
SOURCE_DIR=.
ENGINE_SRC_DIR=$(SOURCE_DIR)/engine
COMMON_SRC_DIR=$(SOURCE_DIR)/common
DLLS_SRC_DIR=$(SOURCE_DIR)/dlls
CL_DLL_SRC_DIR=$(SOURCE_DIR)/cl_dll

INCLUDEDIRS= -I$(PS2_DIR) -I$(DLLS_SRC_DIR) -I$(COMMON_SRC_DIR) -I$(ENGINE_SRC_DIR) -I$(CL_DLL_SRC_DIR)

CDLL_OBJS = \
	$(CL_DLL_SRC_DIR)/hud.o \
	$(CL_DLL_SRC_DIR)/ammo.o \
	$(CL_DLL_SRC_DIR)/ammo_secondary.o \
	$(CL_DLL_SRC_DIR)/ammohistory.o \
	$(CL_DLL_SRC_DIR)/battery.o \
	$(CL_DLL_SRC_DIR)/cdll_int.o \
	$(CL_DLL_SRC_DIR)/death.o \
	$(CL_DLL_SRC_DIR)/flashlight.o \
	$(CL_DLL_SRC_DIR)/geiger.o \
	$(CL_DLL_SRC_DIR)/health.o \
	$(CL_DLL_SRC_DIR)/hud_msg.o \
	$(CL_DLL_SRC_DIR)/hud_redraw.o \
	$(CL_DLL_SRC_DIR)/hud_update.o \
	$(CL_DLL_SRC_DIR)/menu.o \
	$(CL_DLL_SRC_DIR)/message.o \
	$(CL_DLL_SRC_DIR)/MOTD.o \
	$(CL_DLL_SRC_DIR)/parsemsg.o \
	$(CL_DLL_SRC_DIR)/saytext.o \
	$(CL_DLL_SRC_DIR)/scoreboard.o \
	$(CL_DLL_SRC_DIR)/status_icons.o \
	$(CL_DLL_SRC_DIR)/statusbar.o \
	$(CL_DLL_SRC_DIR)/text_message.o \
	$(CL_DLL_SRC_DIR)/train.o \
	$(CL_DLL_SRC_DIR)/util.o

HLDLL_OBJS = \
	$(DLLS_SRC_DIR)/aflock.o \
	$(DLLS_SRC_DIR)/agrunt.o \
	$(DLLS_SRC_DIR)/airtank.o \
	$(DLLS_SRC_DIR)/animating.o \
	$(DLLS_SRC_DIR)/animation.o \
	$(DLLS_SRC_DIR)/apache.o \
	$(DLLS_SRC_DIR)/barnacle.o \
	$(DLLS_SRC_DIR)/barney.o \
	$(DLLS_SRC_DIR)/bigmomma.o \
	$(DLLS_SRC_DIR)/bloater.o \
	$(DLLS_SRC_DIR)/bmodels.o \
	$(DLLS_SRC_DIR)/bullsquid.o \
	$(DLLS_SRC_DIR)/buttons.o \
	$(DLLS_SRC_DIR)/cbase.o \
	$(DLLS_SRC_DIR)/client.o \
	$(DLLS_SRC_DIR)/combat.o \
	$(DLLS_SRC_DIR)/controller.o \
	$(DLLS_SRC_DIR)/crossbow.o \
	$(DLLS_SRC_DIR)/crowbar.o \
	$(DLLS_SRC_DIR)/defaultai.o \
	$(DLLS_SRC_DIR)/doors.o \
	$(DLLS_SRC_DIR)/effects.o \
	$(DLLS_SRC_DIR)/egon.o \
	$(DLLS_SRC_DIR)/explode.o \
	$(DLLS_SRC_DIR)/flyingmonster.o \
	$(DLLS_SRC_DIR)/func_break.o \
	$(DLLS_SRC_DIR)/func_tank.o \
	$(DLLS_SRC_DIR)/game.o \
	$(DLLS_SRC_DIR)/gamerules.o \
	$(DLLS_SRC_DIR)/gargantua.o \
	$(DLLS_SRC_DIR)/gauss.o \
	$(DLLS_SRC_DIR)/genericmonster.o \
	$(DLLS_SRC_DIR)/ggrenade.o \
	$(DLLS_SRC_DIR)/globals.o \
	$(DLLS_SRC_DIR)/glock.o \
	$(DLLS_SRC_DIR)/gman.o \
	$(DLLS_SRC_DIR)/h_ai.o \
	$(DLLS_SRC_DIR)/handgrenade.o \
	$(DLLS_SRC_DIR)/hassassin.o \
	$(DLLS_SRC_DIR)/h_battery.o \
	$(DLLS_SRC_DIR)/h_cine.o \
	$(DLLS_SRC_DIR)/h_cycler.o \
	$(DLLS_SRC_DIR)/headcrab.o \
	$(DLLS_SRC_DIR)/healthkit.o \
	$(DLLS_SRC_DIR)/h_export.o \
	$(DLLS_SRC_DIR)/hgrunt.o \
	$(DLLS_SRC_DIR)/hornet.o \
	$(DLLS_SRC_DIR)/hornetgun.o \
	$(DLLS_SRC_DIR)/houndeye.o \
	$(DLLS_SRC_DIR)/ichthyosaur.o \
	$(DLLS_SRC_DIR)/islave.o \
	$(DLLS_SRC_DIR)/items.o \
	$(DLLS_SRC_DIR)/leech.o \
	$(DLLS_SRC_DIR)/lights.o \
	$(DLLS_SRC_DIR)/maprules.o \
	$(DLLS_SRC_DIR)/monstermaker.o \
	$(DLLS_SRC_DIR)/monsters.o \
	$(DLLS_SRC_DIR)/monsterstate.o \
	$(DLLS_SRC_DIR)/mortar.o \
	$(DLLS_SRC_DIR)/mp5.o \
	$(DLLS_SRC_DIR)/multiplay_gamerules.o \
	$(DLLS_SRC_DIR)/nihilanth.o \
	$(DLLS_SRC_DIR)/nodes.o \
	$(DLLS_SRC_DIR)/osprey.o \
	$(DLLS_SRC_DIR)/pathcorner.o \
	$(DLLS_SRC_DIR)/plane.o \
	$(DLLS_SRC_DIR)/plats.o \
	$(DLLS_SRC_DIR)/player.o \
	$(DLLS_SRC_DIR)/python.o \
	$(DLLS_SRC_DIR)/rat.o \
	$(DLLS_SRC_DIR)/roach.o \
	$(DLLS_SRC_DIR)/rpg.o \
	$(DLLS_SRC_DIR)/satchel.o \
	$(DLLS_SRC_DIR)/schedule.o \
	$(DLLS_SRC_DIR)/scientist.o \
	$(DLLS_SRC_DIR)/scripted.o \
	$(DLLS_SRC_DIR)/shotgun.o \
	$(DLLS_SRC_DIR)/singleplay_gamerules.o \
	$(DLLS_SRC_DIR)/skill.o \
	$(DLLS_SRC_DIR)/sound.o \
	$(DLLS_SRC_DIR)/soundent.o \
	$(DLLS_SRC_DIR)/spectator.o \
	$(DLLS_SRC_DIR)/squadmonster.o \
	$(DLLS_SRC_DIR)/squeakgrenade.o \
	$(DLLS_SRC_DIR)/subs.o \
	$(DLLS_SRC_DIR)/talkmonster.o \
	$(DLLS_SRC_DIR)/teamplay_gamerules.o \
	$(DLLS_SRC_DIR)/tempmonster.o \
	$(DLLS_SRC_DIR)/tentacle.o \
	$(DLLS_SRC_DIR)/triggers.o \
	$(DLLS_SRC_DIR)/tripmine.o \
	$(DLLS_SRC_DIR)/turret.o \
	$(DLLS_SRC_DIR)/util.o \
	$(DLLS_SRC_DIR)/weapons.o \
	$(DLLS_SRC_DIR)/world.o \
	$(DLLS_SRC_DIR)/xen.o \
	$(DLLS_SRC_DIR)/zombie.o \
######################################

EE_OBJS = ps2/main.o \
ps2/sys_ps2.o \
ps2/mips.o \
ps2/ps2mem.o \
ps2/common.o \
$(CDLL_OBJS)
#$(HLDLL_OBJS)

ifeq ($(PS2_BUILD), 1)
# PS2
BASE_ADDRESS = 0x800000
else
# PC
BASE_ADDRESS = 0x02100000
endif

ifeq ($(PS2_BUILD), 1)
EE_LINKFILE = linkfile_ps2
else
EE_LINKFILE = linkfile
endif

EE_CFLAGS = $(INCLUDEDIRS) -Wno-comment -Wno-write-strings -fno-zero-initialized-in-bss -O0
EE_CXXFLAGS = $(INCLUDEDIRS) -Wno-comment -Wno-reorder -Wno-write-strings -fno-use-cxa-atexit -fno-rtti -fno-exceptions -fno-zero-initialized-in-bss -O0

EE_LDFLAGS = -Wl,--entry=init -Wl,-Map,$(EE_BIN_NAME).map -Wl,'--defsym=BASE_ADDRESS=$(BASE_ADDRESS)' 

all: $(EE_BIN)

clean:
	-rm -f $(EE_BIN_NAME).map
	-rm -f $(EE_OBJS)
	-rm -f $(EE_BIN)

PS2SDK = external/ps2sdk/ps2sdk

include $(PS2SDK)/samples/Makefile.pref
ifeq ($(PS2_BUILD), 1)
include eemakefile.eeglobal
else
include $(PS2SDK)/samples/Makefile.eeglobal
endif