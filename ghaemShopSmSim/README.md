# ghaemShopSmSim
arduino SMS Remote Controler over ghaemShopSmSim protocol

 *
 * Be name KHODA (In the Name of ALLAH)
 * 
 
ghaemShop.ir GSM SMS SIM Library <br/>
ghaemShopSmSim.V.3.1.0 <br/>
@1396-02-31<br/>
by Akbar Ghaedi from ghaemShop.ir<br/>
<br/>
http://ghaemShop.ir<br/>
<br/>
http://Ghaem24.ir<br/>
http://MikroTek.ir<br/>
http://Micro24.ir<br/>
http://GhaemComputer.ir<br/>
@1394-09-24..@1396-xx-xx<br/>
<br/>
  * Salavat Hadye be Agha Emam Zaman(GHAEM AALE MOHAMMAD)<br/>
<br/>
you can download last version from:<br/>
  http://www.ghemShop.ir/opensource/arduino/ghaemShopSmSim<br/>
  https://github.com/ghaemshop/ghaemShopSmSim<br/>
<br/>
<br/>
For information on installing libraries, see: http://www.arduino.cc/en/Guide/Libraries<br/>
<br/>
ghaemShopSmSim protocol:<br/>
ghaemShopSmSim v.3.1.0 protocol:<br/>
/*******************************************************************************
	
	
	SetPassword()
		
		Command -> *PASS=*Current Password*New Password*Confirm#
		
		/*
		* Parameters[0] = Command = "PASS="
		* Parameters[1] = Current Password
		* Parameters[2] = New Password
		* Parameters[3] = Confirm (No = 0, Yes > 0 : Delay(ms))
		*/



	SetRelay()
	
		Command -> *RLY=*Current Password*Relay Number or Name*New Value*Confirm#
		Sample  -> *RLY=*pass*1*1*3000#
		Sample  -> *RLY=*pass*Motor*ON*3000#
		
		/*
		* Parameters[0] = Command = "RLY="
		* Parameters[1] = Current Password
		* Parameters[2] = Relay Number or Name
		* Parameters[3] = New Value (0:OFF, 1:ON)
		* Parameters[4] = Confirm (No = 0, Yes > 0 : Delay(ms))
		*/

		
		
	GetRelay()
	
		Command -> *RLY?*Current Password*Relay Number or Name#
		Sample  -> *RLY?*pass*1#
		Sample  -> *RLY?*pass*Motor#

		/*
		* Parameters[0] = Command = "RLY?"
		* Parameters[1] = Current Password
		* Parameters[2] = Relay Number
		* Out Message = Current Value (0:OFF, 1:ON)
		*/

		
		
	SetVariable()
	
		Command -> *VAR=*Current Password*Variable Number or Name*New Value*Confirm#
		Sample  -> *VAR=*pass*1*10*3000#
		Sample  -> *VAR=*pass*MotorSpeed*10*3000#

		/*
		* Parameters[0] = Command = "VAR="
		* Parameters[1] = Current Password
		* Parameters[2] = Variable Name OR Number [0..n]
		* Parameters[3] = New Value (depend on type)
		* Parameters[4] = Confirm (No = 0, Yes > 0 : Delay(ms))
		*/

		
		
	GetVariable()
	
		Command -> *VAR?*Current Password*Relay Number or Name#
		Sample  -> *VAR?*pass*1#
		Sample  -> *VAR?*pass*Temperture#

		/*
		* Parameters[0] = Command = "VAR?"
		* Parameters[1] = Current Password
		* Parameters[2] = Variable Name OR Number [0..n]
		*/

*******************************************************************************/

