// EnergyPlus, Copyright (c) 1996-2016, The Board of Trustees of the University of Illinois and
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy). All rights
// reserved.
//
// If you have questions about your rights to use or distribute this software, please contact
// Berkeley Lab's Innovation & Partnerships Office at IPO@lbl.gov.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without Lawrence Berkeley National Laboratory's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the
// features, functionality or performance of the source code ("Enhancements") to anyone; however,
// if you choose to make your Enhancements available either publicly, or directly to Lawrence
// Berkeley National Laboratory, without imposing a separate written license agreement for such
// Enhancements, then you hereby grant the following license: a non-exclusive, royalty-free
// perpetual license to install, use, modify, prepare derivative works, incorporate into other
// computer software, distribute, and sublicense such enhancements or derivative works thereof,
// in binary and source code form.

// EnergyPlus::ThermalComfort Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// ObjexxFCL Headers
#include <ObjexxFCL/gio.hh>

// EnergyPlus Headers
#include "Fixtures/EnergyPlusFixture.hh"
#include <EnergyPlus/ThermalComfort.hh>
#include <EnergyPlus/UtilityRoutines.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/DataZoneEnergyDemands.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataRoomAirModel.hh>
#include <EnergyPlus/InternalHeatGains.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/SimulationManager.hh>

using namespace EnergyPlus;
using namespace EnergyPlus::ThermalComfort;
using namespace EnergyPlus::DataEnvironment;
using namespace EnergyPlus::DataGlobals;
using namespace EnergyPlus::DataHeatBalance;
using namespace EnergyPlus::DataHVACGlobals;
using namespace EnergyPlus::DataRoomAirModel;
using namespace EnergyPlus::DataHeatBalFanSys;
using namespace EnergyPlus::InternalHeatGains;
using namespace EnergyPlus::HeatBalanceManager;
using namespace EnergyPlus::OutputProcessor;
using namespace EnergyPlus::ScheduleManager;
using namespace SimulationManager;
using namespace ObjexxFCL;

using DataZoneEnergyDemands::ZoneSysEnergyDemand;


TEST_F( EnergyPlusFixture, ThermalComfort_CalcIfSetPointMetTest1 )
{
	NumOfZones = 1;
	ZoneSysEnergyDemand.allocate( NumOfZones );
	ThermalComfortSetPoint.allocate( NumOfZones );
	TempControlType.allocate( 1 );
	AirModel.allocate( NumOfZones );
	AirModel( 1 ).AirModelType = RoomAirModel_Mixing;
	ZTAV.allocate( NumOfZones );
	ZoneThermostatSetPointLo.allocate( NumOfZones );
	ZoneThermostatSetPointHi.allocate( NumOfZones );
	TimeStepZone = 0.25;
	ThermalComfortInASH55.allocate( NumOfZones );
	ThermalComfortInASH55( 1 ).ZoneIsOccupied = true;

	// SingleHeatingSetPoint thermostat

	TempControlType( 1 ) = SingleHeatingSetPoint;

	//heating
	ZTAV( 1 ) = 21.1; // 70F
	ZoneThermostatSetPointLo( 1 ) = 22.2; // 72F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = 500.0; // must be greater than zero
	CalcIfSetPointMet();
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );

	//cooling
	ZTAV( 1 ) = 25.0; // 77F
	ZoneThermostatSetPointHi( 1 ) = 23.9; // 75F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = -500.0; // must be less than zero
	CalcIfSetPointMet();
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );

	// SingleCoolingSetPoint thermostat

	TempControlType( 1 ) = SingleCoolingSetPoint;

	//heating
	ZTAV( 1 ) = 21.1; // 70F
	ZoneThermostatSetPointLo( 1 ) = 22.2; // 72F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = 500.0; // must be greater than zero
	CalcIfSetPointMet();
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );

	//cooling
	ZTAV( 1 ) = 25.0; // 77F
	ZoneThermostatSetPointHi( 1 ) = 23.9; // 75F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = -500.0; // must be less than zero
	CalcIfSetPointMet();
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );

	// SingleHeatCoolSetPoint thermostat

	TempControlType( 1 ) = SingleHeatCoolSetPoint;

	//heating
	ZTAV( 1 ) = 21.1; // 70F
	ZoneThermostatSetPointLo( 1 ) = 22.2; // 72F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = 500.0; // must be greater than zero
	CalcIfSetPointMet();
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );

	//cooling
	ZTAV( 1 ) = 25.0; // 77F
	ZoneThermostatSetPointHi( 1 ) = 23.9; // 75F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = -500.0; // must be less than zero
	CalcIfSetPointMet();
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );

	// DualSetPointWithDeadBand thermostat

	TempControlType( 1 ) = DualSetPointWithDeadBand;

	//heating
	ZTAV( 1 ) = 21.1; // 70F
	ZoneThermostatSetPointLo( 1 ) = 22.2; // 72F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = 500.0; // must be greater than zero
	CalcIfSetPointMet();
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( 0., ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );

	//cooling
	ZTAV( 1 ) = 25.0; // 77F
	ZoneThermostatSetPointHi( 1 ) = 23.9; // 75F
	ZoneSysEnergyDemand( 1 ).TotalOutputRequired = -500.0; // must be less than zero
	CalcIfSetPointMet();
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeating );
	EXPECT_EQ( 0, ThermalComfortSetPoint( 1 ).notMetHeatingOccupied );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetCooling );
	EXPECT_EQ( TimeStepZone, ThermalComfortSetPoint( 1 ).notMetCoolingOccupied );
}

TEST_F( EnergyPlusFixture, ThermalComfort_CalcThermalComfortFanger )
{

	std::string const idf_objects = delimited_string( {
		"Version,8.3;",
		"  People,                                                                 ",
		"    Space People,   !- Name                                      ",
		"    Space,     !- Zone or ZoneList Name                     ",
		"    PeopleSchedule,          !- Number of People Schedule Name            ",
		"    People,                  !- Number of People Calculation Method       ",
		"    5.0,                     !- Number of People                          ",
		"    ,                        !- People per Zone Floor Area {person/m2}    ",
		"    ,                        !- Zone Floor Area per Person {m2/person}    ",
		"    0.3,                     !- Fraction Radiant                          ",
		"    AUTOCALCULATE,           !- Sensible Heat Fraction                    ",
		"    Activity Schedule,       !- Activity Level Schedule Name              ",
		"    ,                        !- Carbon Dioxide Generation Rate {m3/s-W}   ",
		"    Yes,                     !- Enable ASHRAE 55 Comfort Warnings         ",
		"    ZoneAveraged,            !- Mean Radiant Temperature Calculation Type ",
		"    ,                        !- Surface Name/Angle Factor List Name       ",
		"    Work efficiency,         !- Work Efficiency Schedule Name             ",
		"    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
		"    ,                        !- Clothing Insulation Calculation Method Sch",
		"    Clothing Schedule,       !- Clothing Insulation Schedule Name         ",
		"    AirVelocitySchedule,     !- Air Velocity Schedule Name                ",
		"    Fanger;                  !- Thermal Comfort Model 1 Type              ",
		"                                                                          ",
		"  Schedule:Compact,                                                       ",
		"    PeopleSchedule,          !- Name                                      ",
		"    Any Number,              !- Schedule Type Limits Name                 ",
		"    Through: 12/30,          !- Field 1                                   ",
		"    For: AllDays,            !- Field 2                                   ",
		"    Until: 24:00,1.0,        !- Field 3                                   ",
		"    Through: 12/31,          !- Field 1                                   ",
		"    For: AllDays,            !- Field 2                                   ",
		"    Until: 24:00,0.3;        !- Field 3                                   ",
		"                                                                          ",
		"  Schedule:Compact,                                                       ",
		"    Activity Schedule,       !- Name                                      ",
		"    Any Number,              !- Schedule Type Limits Name                 ",
		"    Through: 12/31,          !- Field 1                                   ",
		"    For: AllDays,            !- Field 2                                   ",
		"    Until: 24:00,70;         !- Field 3                                   ",
		"                                                                          ",
		"  Schedule:Compact,                                                       ",
		"    Clothing Schedule,       !- Name                                      ",
		"    Any Number,              !- Schedule Type Limits Name                 ",
		"    Through: 12/31,          !- Field 9                                   ",
		"    For: AllDays,            !- Field 10                                  ",
		"    Until: 24:00,1.0;         !- Field 11                                 ",
		"                                                                          ",
		"  Schedule:Compact,                                                       ",
		"    AirVelocitySchedule,     !- Name                                      ",
		"    Any Number,              !- Schedule Type Limits Name                 ",
		"    Through: 12/31,          !- Field 1                                   ",
		"    For: AllDays,            !- Field 2                                   ",
		"    Until: 24:00,0.0;        !- Field 3                                   ",
		"                                                                          ",
		"  Schedule:Compact,                                                       ",
		"    Work efficiency,         !- Name                                      ",
		"    Any Number,              !- Schedule Type Limits Name                 ",
		"    Through: 12/31,          !- Field 9                                   ",
		"    For: AllDays,            !- Field 10                                  ",
		"    Until: 24:00,0.0;         !- Field 11                                 ",
		"                                                                          ",
		" Output:Diagnostics, DisplayExtraWarnings;",
		" Timestep, 4;",
		" BUILDING, AirloopHVAC_VentilationRateProcedure, 0.0, Suburbs, .04, .4, FullExterior, 25, 6;",
		" SimulationControl, NO, NO, NO, YES, NO;",
		"ScheduleTypeLimits,",
		"  Any Number;              !- Name",
		"  Site:Location,",
		"    Miami Intl Ap FL USA TMY3 WMO=722020E,    !- Name",
		"    25.82,                 !- Latitude {deg}",
		"    -80.30,                !- Longitude {deg}",
		"    -5.00,                 !- Time Zone {hr}",
		"    11;                    !- Elevation {m}",

		"SizingPeriod:DesignDay,",
		" Miami Intl Ap Ann Clg .4% Condns DB/MCWB, !- Name",
		" 7,                        !- Month",
		" 21,                       !- Day of Month",
		" SummerDesignDay,          !- Day Type",
		" 31.7,                     !- Maximum Dry - Bulb Temperature{ C }",
		" 10.0,                      !- Daily Dry - Bulb Temperature Range{ deltaC }",
		" ,                         !- Dry - Bulb Temperature Range Modifier Type",
		" ,                         !- Dry - Bulb Temperature Range Modifier Day Schedule Name",
		" Wetbulb,                  !- Humidity Condition Type",
		" 22.7,                     !- Wetbulb or DewPoint at Maximum Dry - Bulb{ C }",
		" ,                         !- Humidity Condition Day Schedule Name",
		" ,                         !- Humidity Ratio at Maximum Dry - Bulb{ kgWater / kgDryAir }",
		" ,                         !- Enthalpy at Maximum Dry - Bulb{ J / kg }",
		" ,                         !- Daily Wet - Bulb Temperature Range{ deltaC }",
		" 101217.,                  !- Barometric Pressure{ Pa }",
		" 3.8,                      !- Wind Speed{ m / s }",
		" 340,                      !- Wind Direction{ deg }",
		" No,                       !- Rain Indicator",
		" No,                       !- Snow Indicator",
		" No,                       !- Daylight Saving Time Indicator",
		" ASHRAEClearSky,           !- Solar Model Indicator",
		" ,                         !- Beam Solar Day Schedule Name",
		" ,                         !- Diffuse Solar Day Schedule Name",
		" ,                         !- ASHRAE Clear Sky Optical Depth for Beam Irradiance( taub ) { dimensionless }",
		" ,                         !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance( taud ) { dimensionless }",
		" 1.00;                     !- Sky Clearness",

		"SizingPeriod:DesignDay,",
		" Miami Intl Ap Ann Htg 99.6% Condns DB, !- Name",
		" 1,                        !- Month",
		" 21,                       !- Day of Month",
		" WinterDesignDay,          !- Day Type",
		" 8.7,                      !- Maximum Dry - Bulb Temperature{ C }",
		" 0.0,                      !- Daily Dry - Bulb Temperature Range{ deltaC }",
		" ,                         !- Dry - Bulb Temperature Range Modifier Type",
		" ,                         !- Dry - Bulb Temperature Range Modifier Day Schedule Name",
		" Wetbulb,                  !- Humidity Condition Type",
		" 8.7,                      !- Wetbulb or DewPoint at Maximum Dry - Bulb{ C }",
		" ,                         !- Humidity Condition Day Schedule Name",
		" ,                         !- Humidity Ratio at Maximum Dry - Bulb{ kgWater / kgDryAir }",
		" ,                         !- Enthalpy at Maximum Dry - Bulb{ J / kg }",
		" ,                         !- Daily Wet - Bulb Temperature Range{ deltaC }",
		" 101217.,                  !- Barometric Pressure{ Pa }",
		" 3.8,                      !- Wind Speed{ m / s }",
		" 340,                      !- Wind Direction{ deg }",
		" No,                       !- Rain Indicator",
		" No,                       !- Snow Indicator",
		" No,                       !- Daylight Saving Time Indicator",
		" ASHRAEClearSky,           !- Solar Model Indicator",
		" ,                         !- Beam Solar Day Schedule Name",
		" ,                         !- Diffuse Solar Day Schedule Name",
		" ,                         !- ASHRAE Clear Sky Optical Depth for Beam Irradiance( taub ) { dimensionless }",
		" ,                         !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance( taud ) { dimensionless }",
		" 0.00;                     !- Sky Clearness",

		"Zone,",
		"  Space,                   !- Name",
		"  0.0000,                  !- Direction of Relative North {deg}",
		"  0.0000,                  !- X Origin {m}",
		"  0.0000,                  !- Y Origin {m}",
		"  0.0000,                  !- Z Origin {m}",
		"  1,                       !- Type",
		"  1,                       !- Multiplier",
		"  2.4,                     !- Ceiling Height {m}",
		"  ,                        !- Volume {m3}",
		"  autocalculate,           !- Floor Area {m2}",
		"  ,                        !- Zone Inside Convection Algorithm",
		"  ,                        !- Zone Outside Convection Algorithm",
		"  Yes;                     !- Part of Total Floor Area",

		"ZoneGroup,",
		" Zone Group,               !- Name",
		" Zone List,                !- Zone List Name",
		" 10;                       !- Zone List Multiplier",

		"ZoneList,",
		" Zone List,                !- Name",
		" Spacex10;                 !- Zone 1 Name",

		"Zone,",
		"  Spacex10,                !- Name",
		"  0.0000,                  !- Direction of Relative North {deg}",
		"  0.0000,                  !- X Origin {m}",
		"  0.0000,                  !- Y Origin {m}",
		"  0.0000,                  !- Z Origin {m}",
		"  1,                       !- Type",
		"  1,                       !- Multiplier",
		"  2.4,                     !- Ceiling Height {m}",
		"  ,                        !- Volume {m3}",
		"  autocalculate,           !- Floor Area {m2}",
		"  ,                        !- Zone Inside Convection Algorithm",
		"  ,                        !- Zone Outside Convection Algorithm",
		"  Yes;                     !- Part of Total Floor Area",

		"People,",
		" Spacex10 People,          !- Name",
		" Spacex10,                 !- Zone or ZoneList Name",
		" OnSched,                  !- Number of People Schedule Name",
		" people,                   !- Number of People Calculation Method",
		" 11,                       !- Number of People",
		" ,                         !- People per Zone Floor Area{ person / m2 }",
		" ,                         !- Zone Floor Area per Person{ m2 / person }",
		" 0.3,                      !- Fraction Radiant",
		" AutoCalculate,            !- Sensible Heat Fraction",
		" Activity Schedule;        !- Activity Level Schedule Name",

		"Lights,",
		" Space Lights,             !- Name",
		" Space,                    !- Zone or ZoneList Name",
		" OnSched,                  !- Schedule Name",
		" Watts/Area,               !- Design Level Calculation Method",
		" ,                         !- Lighting Level{ W }",
		" 10.0,                     !- Watts per Zone Floor Area{ W / m2 }",
		" ,                         !- Watts per Person{ W / person }",
		" 0.1,                      !- Return Air Fraction",
		" 0.59,                     !- Fraction Radiant",
		" 0.2,                      !- Fraction Visible",
		" 0,                        !- Fraction Replaceable",
		" GeneralLights;            !- End - Use Subcategory",

		"Lights,",
		" Space Lights x10,         !- Name",
		" Spacex10,                 !- Zone or ZoneList Name",
		" OnSched,                  !- Schedule Name",
		" Watts/Area,               !- Design Level Calculation Method",
		" ,                         !- Lighting Level{ W }",
		" 10.0,                     !- Watts per Zone Floor Area{ W / m2 }",
		" ,                         !- Watts per Person{ W / person }",
		" 0.1,                      !- Return Air Fraction",
		" 0.59,                     !- Fraction Radiant",
		" 0.2,                      !- Fraction Visible",
		" 0,                        !- Fraction Replaceable",
		" GeneralLights;            !- End - Use Subcategory",

		"ElectricEquipment,",
		" Space ElecEq,             !- Name",
		" Space,                    !- Zone or ZoneList Name",
		" OnSched,                  !- Schedule Name",
		" Watts/Area,               !- Design Level Calculation Method",
		" ,                         !- Design Level{ W }",
		" 20.0,                     !- Watts per Zone Floor Area{ W / m2 }",
		" ,                         !- Watts per Person{ W / person }",
		" 0.1,                      !- Fraction Latent",
		" 0.3,                      !- Fraction Radiant",
		" 0.1;                      !- Fraction Lost",

		"ElectricEquipment,",
		" Space ElecEq x10,         !- Name",
		" Spacex10,                 !- Zone or ZoneList Name",
		" OnSched,                  !- Schedule Name",
		" Watts/Area,               !- Design Level Calculation Method",
		" ,                         !- Design Level{ W }",
		" 20.0,                     !- Watts per Zone Floor Area{ W / m2 }",
		" ,                         !- Watts per Person{ W / person }",
		" 0.1,                      !- Fraction Latent",
		" 0.3,                      !- Fraction Radiant",
		" 0.1;                      !- Fraction Lost",

		"Schedule:Compact,",
		" OnSched,                  !- Name",
		" Fraction,                 !- Schedule Type Limits Name",
		" Through: 12/31,           !- Field 1",
		" For: AllDays,             !- Field 2",
		" Until: 24:00, 1.0;        !- Field 26",

		"ScheduleTypeLimits,",
		" Fraction,                 !- Name",
		" 0.0,                      !- Lower Limit Value",
		" 1.0,                      !- Upper Limit Value",
		" CONTINUOUS;               !- Numeric Type",

		"Construction,",
		" INT-WALL-1,               !- Name",
		" GP02,                     !- Outside Layer",
		" AL21,                     !- Layer 2",
		" GP02;                     !- Layer 3",

		"Material,",
		" GP02,                     !- Name",
		" MediumSmooth,             !- Roughness",
		" 1.5900001E-02,            !- Thickness{ m }",
		" 0.1600000,                !- Conductivity{ W / m - K }",
		" 801.0000,                 !- Density{ kg / m3 }",
		" 837.0000,                 !- Specific Heat{ J / kg - K }",
		" 0.9000000,                !- Thermal Absorptance",
		" 0.7500000,                !- Solar Absorptance",
		" 0.7500000;                !- Visible Absorptance",

		"Material:AirGap,",
		" AL21,                     !- Name",
		" 0.1570000;                !- Thermal Resistance{ m2 - K / W }",

		"Construction,",
		"FLOOR-SLAB-1,              !- Name",
		"CC03,                      !- Outside Layer",
		"CP01;                      !- Layer 2",

		"Material,",
		" CC03,                     !- Name",
		" MediumRough,              !- Roughness",
		" 0.1016000,                !- Thickness{ m }",
		" 1.310000,                 !- Conductivity{ W / m - K }",
		" 2243.000,                 !- Density{ kg / m3 }",
		" 837.0000,                 !- Specific Heat{ J / kg - K }",
		" 0.9000000,                !- Thermal Absorptance",
		" 0.6500000,                !- Solar Absorptance",
		" 0.6500000;                !- Visible Absorptance",

		"Material:NoMass,",
		" CP01,                     !- Name",
		" Rough,                    !- Roughness",
		" 0.3670000,                !- Thermal Resistance{ m2 - K / W }",
		" 0.9000000,                !- Thermal Absorptance",
		" 0.7500000,                !- Solar Absorptance",
		" 0.7500000;                !- Visible Absorptance",

		"Construction,",
		" CLNG-1,                   !- Name",
		" MAT-CLNG-1;               !- Outside Layer",

		"Material:NoMass,",
		" MAT-CLNG-1,               !- Name",
		" Rough,                    !- Roughness",
		" 0.652259290,              !- Thermal Resistance{ m2 - K / W }",
		" 0.65,                     !- Thermal Absorptance",
		" 0.65,                     !- Solar Absorptance",
		" 0.65;                     !- Visible Absorptance",

		"BuildingSurface:Detailed,",
		" FRONT-1,                  !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Space,                    !- Zone Name",
		" Outdoors,                 !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" SunExposed,               !- Sun Exposure",
		" WindExposed,              !- Wind Exposure",
		" 0.50000,                  !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
		" 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
		" 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
		" 30.5, 0.0, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" C1-1,                     !- Name",
		" CEILING,                  !- Surface Type",
		" CLNG-1,                   !- Construction Name",
		" Space,                    !- Zone Name",
		" Outdoors,                 !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
		" 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 2 {m}",
		" 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 3 {m}",
		" 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" F1-1,                     !- Name",
		" FLOOR,                    !- Surface Type",
		" FLOOR-SLAB-1,             !- Construction Name",
		" Space,                    !- Zone Name",
		" Outdoors,                   !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 1 {m}",
		" 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
		" 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
		" 3.7, 3.7, 0.0;            !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" SB12,                     !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Space,                    !- Zone Name",
		" Adiabatic,                !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
		" 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
		" 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
		" 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" SB14,                     !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Space,                    !- Zone Name",
		" Adiabatic,                !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
		" 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
		" 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
		" 0.0, 0.0, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" SB15,                     !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Space,                    !- Zone Name",
		" Adiabatic,                !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 26.8, 3.7, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
		" 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
		" 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
		" 3.7, 3.7, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" FRONT-1x10,               !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Spacex10,                 !- Zone Name",
		" Outdoors,                 !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" SunExposed,               !- Sun Exposure",
		" WindExposed,              !- Wind Exposure",
		" 0.50000,                  !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
		" 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
		" 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
		" 30.5, 0.0, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" C1-1x10,                  !- Name",
		" CEILING,                  !- Surface Type",
		" CLNG-1,                   !- Construction Name",
		" Spacex10,                 !- Zone Name",
		" Outdoors,                 !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
		" 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 2 {m}",
		" 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 3 {m}",
		" 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" F1-1x10,                  !- Name",
		" FLOOR,                    !- Surface Type",
		" FLOOR-SLAB-1,             !- Construction Name",
		" Spacex10,                 !- Zone Name",
		" Outdoors,                   !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 1 {m}",
		" 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
		" 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
		" 3.7, 3.7, 0.0;            !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" SB12x10,                  !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Spacex10,                 !- Zone Name",
		" Adiabatic,                !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
		" 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
		" 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
		" 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" SB14x10,                  !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Spacex10,                 !- Zone Name",
		" Adiabatic,                !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
		" 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
		" 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
		" 0.0, 0.0, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

		"BuildingSurface:Detailed,",
		" SB15x10,                  !- Name",
		" WALL,                     !- Surface Type",
		" INT-WALL-1,               !- Construction Name",
		" Spacex10,                 !- Zone Name",
		" Adiabatic,                !- Outside Boundary Condition",
		" ,                         !- Outside Boundary Condition Object",
		" NoSun,                    !- Sun Exposure",
		" NoWind,                   !- Wind Exposure",
		" 0.0,                      !- View Factor to Ground",
		" 4,                        !- Number of Vertices",
		" 26.8, 3.7, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
		" 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
		" 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
		" 3.7, 3.7, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

		"Output:Table:SummaryReports,",
		"  AllSummary; !- Report 1 Name",
		" ",


	} );

	ASSERT_FALSE( process_idf( idf_objects ) );

	OutputProcessor::TimeValue.allocate( 2 );
	DataGlobals::DDOnlySimulation = true;

	ManageSimulation();

//	compare_err_stream( "" );

	ZTAVComf( 1 ) = 25.0;
	MRT( 1 ) = 26.0;
	ZoneAirHumRatAvgComf( 1 ) = 0.00529; // 0.002 to 0.006

	CalcThermalComfortFanger();

	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPMV, -1.262, 0.005 );
	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPPD, 38.3, 0.1 );

	ZTAVComf( 1 ) = 26.0;
	MRT( 1 ) = 27.0;
	ZoneAirHumRatAvgComf( 1 ) = 0.00529; // 0.002 to 0.006

	CalcThermalComfortFanger();

	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPMV, -0.860, 0.005 );
	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPPD, 20.6, 0.1 );

	ZTAVComf( 1 ) = 27.0;
	MRT( 1 ) = 28.0;
	ZoneAirHumRatAvgComf( 1 ) = 0.00529; // 0.002 to 0.006

	CalcThermalComfortFanger();

	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPMV, -0.460, 0.005 );
	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPPD, 9.4, 0.1 );

	ZTAVComf( 1 ) = 25.0;
	MRT( 1 ) = 26.0;
	ZoneAirHumRatAvgComf( 1 ) = 0.00629; // 0.002 to 0.006

	CalcThermalComfortFanger();

	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPMV, -1.201, 0.005 );
	EXPECT_NEAR( ThermalComfortData( 1 ).FangerPPD, 35.3, 0.1 );
}


