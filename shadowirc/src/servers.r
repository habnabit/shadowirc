/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2001 John Bafford
	dshadow@shadowirc.com
	http://www.shadowirc.com

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "ServerList.r"

//Undernet, EfNet, DalNet, AustNet, SorceryNet, CastleNet, ChatNet, EspritNet, FEFNet, Galaxynet, GammaNet, KronoNet, NewNet, PTNet, SuperChat, ShadowNet, XWorld
resource 'TMPL' (128, "NTWK")
{
	
	{
		"Reserved", 'FWRD',
		"Networks", 'OCNT',
		"Network List", 'LSTC',
		"Network Type", 'TNAM',
		"Reserved", 'FLNG',
		"Network Name", 'P020',
		"", 'LSTE'
	};
};

resource 'TMPL' (129, "SRVR")
{
	{
		"Network Type", 'TNAM',
		"Reserved", 'FWRD',
		"Servers", 'OCNT',
		"Server List", 'LSTC',
		"Port", 'UWRD',
		"Reserved", 'FWRD',
		"Reserved", 'FLNG',
		"Server Name", 'P028',
		"", 'LSTE'
	};
};

resource 'NTWK' (128, "Network List")
{
	{
		'unet', 0, "Undernet",
		'efnt', 0, "EfNet",
		'daln', 0, "DalNet",
		'aftr', 0, "AfterNet",
		'aust', 0, "AustNet",
		'sorc', 0, "SorceryNet",
		'cast', 0, "CastleNet",
		'chat', 0, "ChatNet",
		'drkt', 0, "DarkTree.Net",
		'duhh', 0, "Duh-Net",
		'EICN', 0, "EICN",
		'espr', 0, "EspritNet",
		'fefn', 0, "FEFNet",
		'gala', 0, "Galaxynet",
		'gamm', 0, "GammaNet",
		'kron', 0, "KronoNet",
		'newn', 0, "NewNet",
		'ptnt', 0, "PTNet",
		'star', 0, "StarChat",
		'supc', 0, "SuperChat",
		'shad', 0, "ShadowNet",
		'wolf', 0, "WolfNet",
		'xwor', 0, "XWorld",
		'opnn', 0, "Open Projects Network",
		'qdek', 0, "QDeckers",
		'¥¥¥¥', 0, "Miscelaneous"
	};
};

resource 'SRVR' (128, "Undernet")
{
	'unet',
	{
		6667, 0, 0, "arlington.va.us.undernet.org",
		6667, 0, 0, "austin.tx.us.undernet.org",
		6667, 0, 0, "mclean.va.us.undernet.org",
		6667, 0, 0, "washington.dc.us.undernet.org",
		
		6667, 0, 0, "montreal.qu.ca.undernet.org",
		6667, 0, 0, "toronto.on.ca.undernet.org",

		6667, 0, 0, "amsterdam.nl.eu.undernet.org",
		6667, 0, 0, "brussels.be.eu.undernet.org",
		6667, 0, 0, "caen.fr.eu.undernet.org",
		6667, 0, 0, "diemen.nl.eu.undernet.org",
		6667, 0, 0, "flanders.be.eu.undernet.org",
		6667, 0, 0, "geneva.ch.eu.undernet.org",
		6667, 0, 0, "graz.at.eu.undernet.org",
		6667, 0, 0, "graz2.at.eu.undernet.org",
		6667, 0, 0, "haarlem.nl.eu.undernet.org",
		6667, 0, 0, "london.uk.eu.undernet.org",
		6667, 0, 0, "oslo.no.eu.undernet.org",
		6667, 0, 0, "paris.fr.eu.undernet.org",
		6667, 0, 0, "stockholm.se.eu.undernet.org"
	};
};

resource 'SRVR' (129, "EfNet")
{
	'efnt',
	{
		6667, 0, 0, "irc.cs.cmu.edu",
		6667, 0, 0, "irc.colorado.edu",
		6667, 0, 0, "irc.mindspring.com",
		6667, 0, 0, "irc.anet-stl.com",
		6667, 0, 0, "irc.c-com.net",
		6667, 0, 0, "ircd.txdirect.net",
		6667, 0, 0, "irc.umich.edu",
		6667, 0, 0, "irc.uci.edu",
		6667, 0, 0, "irc.emory.edu",
		6667, 0, 0, "irc.powersurfr.com",
		6667, 0, 0, "irc.blackened.com",
		6667, 0, 0, "irc.sprynet.com",
		6667, 0, 0, "irc.rift.com",
		6667, 0, 0, "irc.mbnet.mb.ca",
		6667, 0, 0, "irc.magic.ca",
		6667, 0, 0, "irc.frontiernet.net",
		6667, 0, 0, "irc.total.net",
		6667, 0, 0, "irc.home.com",
		6667, 0, 0, "irc02.irc.aol.com",
		6667, 0, 0, "irc.psinet.com",
		6667, 0, 0, "irc-1.netcom.com",
		6667, 0, 0, "irc-2.netcom.com",
		6667, 0, 0, "ircd2.netcom.com",
		6667, 0, 0, "irc2-2.netcom.com",
		6667, 0, 0, "irc2-1.netcom.com",
		6667, 0, 0, "ircd.netcom.com",
		6667, 0, 0, "irc.prison.net",
		6667, 0, 0, "irc.mcs.net",
		6667, 0, 0, "irc.stanford.edu",
		6667, 0, 0, "irc.cerf.net",
		6667, 0, 0, "irc.cs.rpi.edu",
		6667, 0, 0, "irc.lightning.net",
		6667, 0, 0, "irc.pacbell.net",
		6667, 0, 0, "irc.exodus.net",
		6667, 0, 0, "irc.best.net",
		6667, 0, 0, "ircd.concentric.net",
		6667, 0, 0, "irc.ais.net",
		6667, 0, 0, "irc.uiuc.edu",
		6667, 0, 0, "irc.umn.edu",

		6667, 0, 0, "irc.nbnet.nb.ca",
		6667, 0, 0, "irc.passport.ca",
		6667, 0, 0, "irc.polymtl.ca",
		6667, 0, 0, "efnet.cs.hut.fi",
		6667, 0, 0, "irc.ec-lille.fr",
		6667, 0, 0, "irc.inter.net.il",
		6667, 0, 0, "irc.nijenrode.nl",
		6667, 0, 0, "irc.homelien.no",
		6667, 0, 0, "efnet.telia.no",
		6667, 0, 0, "irc.ced.chalmers.se",
		6667, 0, 0, "irc.df.lth.se",
		6667, 0, 0, "efnet.demon.co.uk"
	}
};

resource 'SRVR' (130, "DalNet")
{
	'daln',
	{
		6667, 0, 0, "webzone.ok.us.dal.net",
		6667, 0, 0, "barovia.oh.us.dal.net",
		6667, 0, 0, "sodre.fl.us.dal.net",
		6667, 0, 0, "webbernet.mi.us.dal.net",
		6667, 0, 0, "firehouse.tx.us.dal.net",
		6667, 0, 0, "glass.oh.us.dal.net",
		6667, 0, 0, "liberty.nj.us.dal.net",
		6667, 0, 0, "hebron.in.us.dal.net",
		6667, 0, 0, "ans.dc.us.dal.net",
		6667, 0, 0, "mindijari.ca.us.dal.net",
		6667, 0, 0, "voyager.ca.us.dal.net",
		6667, 0, 0, "trapdoor.ca.us.dal.net",
		6667, 0, 0, "indy.in.us.dal.net",
		6667, 0, 0, "qis.md.us.dal.net",
		6667, 0, 0, "chrome.mo.us.dal.net",
		6667, 0, 0, "viper.mo.us.dal.net",
		6667, 0, 0, "stlouis.mo.us.dal.net",

		6667, 0, 0, "opus.bc.ca.dal.net",
		6667, 0, 0, "toronto.on.ca.dal.net",
		6667, 0, 0, "farside.ab.ca.dal.net",

		6667, 0, 0, "defiant.uk.eu.dal.net",
		6667, 0, 0, "viking.no.eu.dal.net",
		6667, 0, 0, "ced.se.eu.dal.net",
		6667, 0, 0, "powertech.no.eu.dal.net",
		6667, 0, 0, "algo.se.eu.dal.net"
	};
};

resource 'SRVR' (131, "SuperChat")
{
	'supc',
	{
		6667, 0, 0, "NewBrunswick.NJ.US.SuperChat.Org",
		6667, 0, 0, "Columbus.OH.US.Superchat.Org",
		6667, 0, 0, "SanAntonio.TX.US.SuperChat.Org",
		6667, 0, 0, "Kentville.NS.CA.SuperChat.Org",
		6667, 0, 0, "Los-Angeles.CA.US.SuperChat.Org",
		6667, 0, 0, "Houston.TX.US.SuperChat.Org",
		6667, 0, 0, "Normal.IL.US.superchat.org",
		6667, 0, 0, "Melbourne.AU.SuperChat.Org"
	};
};

resource 'SRVR' (132, "ShadowNet")
{
	'shad',
	{
		6667, 0, 0, "Floyd.IL.US.ShadowNet.Org",
		6667, 0, 0, "Myst.OR.US.ShadowNet.Org",
		6667, 0, 0, "Bayou.LA.US.ShadowNet.Org",
		6667, 0, 0, "KPNet.IL.US.ShadowNet.Org",
		6667, 0, 0, "Alcatraz.MA.US.ShadowNet.Org",
	}
};

resource 'SRVR' (133, "AustNet")
{
	'aust',
	{
		6667, 0, 0, "landing.ky.us.austnet.org",
		6667, 0, 0, "tavern.oh.us.austnet.org",
		6667, 0, 0, "portland.or.us.austnet.org",
		6667, 0, 0, "explorer.ca.us.austnet.org",
		6667, 0, 0, "tni.se.eu.austnet.org",
		6667, 0, 0, "integrity.uk.eu.austnet.org",
		6667, 0, 0, "aussie.nsw.au.austnet.org",
		6667, 0, 0, "ozemail.nsw.au.austnet.org",
		6667, 0, 0, "iinet.wa.au.austnet.org",
		6667, 0, 0, "firstlink.nsw.au.austnet.org",
		6667, 0, 0, "ausbone.sa.au.austnet.org",
		6667, 0, 0, "ausbone.nsw.au.austnet.org",
		6667, 0, 0, "ausbone.qld.au.austnet.org",
		6667, 0, 0, "wantree.wa.au.austnet.org",
		6667, 0, 0, "frozen.nf.ca.austnet.org",
		6667, 0, 0, "puredata.on.ca.austnet.org",
		6667, 0, 0, "voyager.nz.austnet.org"
	};
};

resource 'SRVR' (134, "SorceryNet")
{
	'sorc',
	{
		6667, 0, 0, "cookie.sorcery.net",
		6667, 0, 0, "nexus.sorcery.net",
		9000, 0, 0, "voyager.sorcery.net",
		6667, 0, 0, "kechara.sorcery.net",
		6667, 0, 0, "outland.sorcery.net",
		9000, 0, 0, "zanzibar.sorcery.net"
	};
};

resource 'SRVR' (135, "CastleNet")
{
	'cast',
	{
		6667, 0, 0, "merlin.CastleNet.Org",
		6667, 0, 0, "Matnet.AK.CastleNet.Org",
		6667, 0, 0, "Bakersfield.CA.CastleNet.Org",
		6667, 0, 0, "Cumulus.CO.CastleNet.Org",
		6667, 0, 0, "SpringSips.CO.CastleNet.Org",
		6667, 0, 0, "Horsetooth.CO.CastleNet.Org",
		6667, 0, 0, "WhiteKnight.IN.CastleNet.Org",
		6667, 0, 0, "Springfield.MA.CastleNet.Org",
		6667, 0, 0, "Memphis.TN.CastleNet.Org",
		6667, 0, 0, "Roanoke.VA.CastleNet.Org",
		6667, 0, 0, "USSR.AU.CastleNet.Org"
	};
};

resource 'SRVR' (136, "ChatNet")
{
	'chat',
	{
		6667, 0, 0, "Chelmsford.MA.US.ChatNet.Org",
		6667, 0, 0, "Chicago.IL.US.ChatNet.Org",
		6667, 0, 0, "Dayton.OH.US.ChatNet.Org",
		6667, 0, 0, "London.UK.EU.ChatNet.Org",
		6667, 0, 0, "LosAngeles.CA.US.ChatNet.Org",
		6667, 0, 0, "Louisville.KY.US.ChatNet.Org",
		6667, 0, 0, "Pensacola.FL.US.ChatNet.Org",
		6667, 0, 0, "Portland.OR.US.Chatnet.Org",
		6667, 0, 0, "RockHill.SC.US.ChatNet.Org",
		6667, 0, 0, "Scranton.PA.US.ChatNet.Org",
		6667, 0, 0, "SF.CA.US.ChatNet.Org",
		6667, 0, 0, "Skien.NO.EU.ChatNet.Org",
		6667, 0, 0, "SLC.UT.US.ChatNet.Org",
		6667, 0, 0, "StLouis.MO.US.Chatnet.Org",
		6667, 0, 0, "Tupelo.MS.US.Chatnet.Org",
		6667, 0, 0, "WalnutCreek.CA.US.ChatNet.Org"
	};
};

resource 'SRVR' (137, "EspritNet")
{
	'espr',
	{
		6667, 0, 0, "Cleveland.OH.US.Esprit.Net",
		6667, 0, 0, "Dallas.TX.US.Esprit.net",
		6667, 0, 0, "Lakewood.OH.US.Esprit.Net",
		6667, 0, 0, "Vancouver.BC.CA.Esprit.Net",
	};
};

resource 'SRVR' (138, "FEFNet")
{
	'fefn',
	{
		6667, 0, 0, "liii.fef.net",
		6667, 0, 0, "medina.fef.net",
		6667, 0, 0, "perth.au.fef.net",
		6667, 0, 0, "quantum.uk.fef.net",
		6667, 0, 0, "services.fef.net",
		6667, 0, 0, "stf.fef.net",
		6667, 0, 0, "sysfail.fef.net",
		6667, 0, 0, "tower.fef.net",
		6667, 0, 0, "transverse.fef.net",
		6667, 0, 0, "vendetta.fef.net",
		6667, 0, 0, "vvm.fef.net",
		6667, 0, 0, "wolfen.fef.net",
		6667, 0, 0, "zoomnet.fef.net",
	};
};

resource 'SRVR' (139, "Galaxynet")
{
	'gala',
	{
		6667, 0, 0, "albany.ny.us.galaxynet.org",
		6667, 0, 0, "atlanta.ga.us.galaxynet.org",
		6667, 0, 0, "BestWeb.US.GalaxyNet.Org",
		6667, 0, 0, "charlotte.nc.us.galaxynet.org",
		6667, 0, 0, "citynet.us.galaxynet.org",
		6667, 0, 0, "CyberStreet.US.GalaxyNet.Org",
		6667, 0, 0, "gymnet.us.galaxynet.org",
		6667, 0, 0, "Honolulu.HI.US.GalaxyNet.Org",
		6667, 0, 0, "Kauai.HI.US.GalaxyNet.Org",
		6667, 0, 0, "Limit.us.galaxynet.org",
		6667, 0, 0, "macon.ga.us.galaxynet.org",
		6667, 0, 0, "marshall.mn.us.GalaxyNet.org",
		6667, 0, 0, "Pookey.us.galaxynet.org",
		6667, 0, 0, "primary.us.galaxynet.org",
		6667, 0, 0, "QuickSilver.US.GalaxyNet.Org",
		6667, 0, 0, "saltlake.ut.us.galaxynet.org",
		6667, 0, 0, "Sedona.AZ.US.GalaxyNet.ORG",
		6667, 0, 0, "sprynet.us.galaxynet.org",
		6667, 0, 0, "Stillwater.OK.US.GalaxyNet.Org",
		6667, 0, 0, "webbernet.us.galaxynet.org",
		6667, 0, 0, "chariot.au.galaxynet.org",
		6667, 0, 0, "online.be.galaxynet.org",
		6667, 0, 0, "Toronto.ON.CA.GalaxyNet.Org",
		6667, 0, 0, "Nicosia.CY.Galaxynet.ORG",
		6667, 0, 0, "netvision.il.galaxynet.org",
		6667, 0, 0, "cybercomm.nl.galaxynet.org",
		6667, 0, 0, "Auckland.NZ.GalaxyNet.Org",
		6667, 0, 0, "Wroclaw.PL.GalaxyNet.org",
		6667, 0, 0, "Moscow.RU.GalaxyNET.org",
		6667, 0, 0, "pacific.sg.galaxynet.org",
		6667, 0, 0, "pacific2.sg.galaxynet.org",
		6667, 0, 0, "Paris.Fr.Galaxynet.Org",
		6667, 0, 0, "Swiftech.SG.GalaxyNet.Org",
		6667, 0, 0, "SINGAPORE.sg.Galaxynet.org",
		6667, 0, 0, "telia.no.galaxynet.org",
		6667, 0, 0, "VLTmedia.SE.GalaxyNet.Org",
		6667, 0, 0, "bristol.uk.galaxynet.org",
		6667, 0, 0, "Nottingham.UK.GalaxyNet.Org"
	};
};

resource 'SRVR' (140, "GammaNet")
{
	'gamm',
	{
		6667, 0, 0, "ElGuapo.UT.US.Gamma.NET",
		6667, 0, 0, "Firefly.VA.US.Gamma.Net",
		6667, 0, 0, "Frontier.CA.US.Gamma.NET",
		6667, 0, 0, "Galaxy.IL.US.Gamma.NET",
		6667, 0, 0, "Pluto.CT.US.Gamma.Net",
		6667, 0, 0, "projects.ut.us.gamma.net",
		6667, 0, 0, "SaltLake.UT.US.Gamma.NET",
		6667, 0, 0, "Venus.IL.US.Gamma.NET",
	};
};

resource 'SRVR' (141, "KronoNet")
{
	'kron',
	{
		6667, 0, 0, "Highestspeed.Krono.Net",
		6667, 0, 0, "KeyWest.Krono.net",
		6667, 0, 0, "services.krono.net",
		6667, 0, 0, "SunShine.Krono.Net",
		6667, 0, 0, "wolf.krono.net",
	};
};


resource 'SRVR' (142, "NewNet")
{
	'newn',
	{
		6667, 0, 0, "irc.aohell.org",
		6667, 0, 0, "irc.assassination.org",
		6667, 0, 0, "irc.aye.net",
		6667, 0, 0, "irc.busprod.com",
		6667, 0, 0, "irc.dragondata.com",
		6667, 0, 0, "irc.eskimo.com",
		6667, 0, 0, "irc.fastlane.net",
		6667, 0, 0, "irc.gaianet.net",
		6667, 0, 0, "irc.interaccess.net",
		6667, 0, 0, "irc.intergate.bc.ca",
		6667, 0, 0, "irc.jaxn.com",
		6667, 0, 0, "irc.keytech.com",
		6667, 0, 0, "irc.klis.com",
		6667, 0, 0, "irc.ncplus.com",
		6667, 0, 0, "irc.para-net.com",
		6667, 0, 0, "irc.salgsnett.no",
		6667, 0, 0, "irc.sprintlink.co.za",
		6667, 0, 0, "irc.tscnet.com",
		6667, 0, 0, "newnet.grolier.fr",
		6667, 0, 0, "newnet.online.be",
		6667, 0, 0, "newnet.telia.NO",
		6667, 0, 0, "security.dragondata.com",
	};
};

resource 'SRVR' (143, "PTNet")
{
	'ptnt',
	{
		6667, 0, 0, "irc.ualg.pt",
		6667, 0, 0, "irc.isec.pt",
		6667, 0, 0, "irc.ist.utl.pt",
		6667, 0, 0, "irc.eunet.pt",
		6667, 0, 0, "irc.esoterica.pt",
		6667, 0, 0, "irc.iscte.pt",
		6667, 0, 0, "irc.madinfo.pt",
		6667, 0, 0, "irc.dei.isep.ipp.pt",
		6667, 0, 0, "irc.uc.pt",
		6667, 0, 0, "irc.ubi.pt",
		6667, 0, 0, "irc.ua.pt",
		6667, 0, 0, "irc.ufp.pt",
		6667, 0, 0, "irc.aaum.pt",
		6667, 0, 0, "irc.fe.up.pt",
		6667, 0, 0, "irc.telepac.pt",
		6667, 0, 0, "irc.isel.pt",
		6667, 0, 0, "irc.ip.pt",
		6667, 0, 0, "irc.utad.pt",
		6667, 0, 0, "irc.rccn.net",
	};
};


resource 'SRVR' (144, "XWorld")
{
	'xwor',
	{
		6667, 0, 0, "Bayern.DE.EU.XWorld.org",
		6667, 0, 0, "Kansas-City.KS.US.XWorld.Org",
		6667, 0, 0, "Marietta.OH.US.XWorld.org",
		6667, 0, 0, "Pasadena.CA.US.XWorld.Org",
		6667, 0, 0, "RockHill.SC.US.Xworld.Org",
		6667, 0, 0, "Telford.UK.EU.XWorld.Org",
		6667, 0, 0, "Wesel.DE.EU.XWorld.Org",
	};
};

resource 'SRVR' (145, "Miscelaneous")
{
	'¥¥¥¥',
	{
		6667, 0, 0, "chat.talkcity.com"
	};
};


resource 'SRVR' (146, "Duh-Net")
{
	'duhh',
	{
		6667, 0, 0, "Sedalia.MO.US.Duh-Net.org",
		6667, 0, 0, "Dallas.TX.US.Duh-Net.org",
		6667, 0, 0, "Hollywood.CA.US.Duh-Net.org",
	};
};

//updated 2001-11-11 "ShawnH" <ShawnH@AfterNET.Org>
resource 'SRVR' (147, "AfterNet")
{
	'aftr',
	{
		6667, 0, 0, "irc.AfterNET.Org",
		6667, 0, 0, "ny.us.AfterNET.Org",
		6667, 0, 0, "boston.AfterNET.Org",
		6667, 0, 0, "ic5.eu.AfterNET.Org",
		6667, 0, 0, "baltimore.md.AfterNET.Org"
	};
};

resource 'SRVR' (148, "EICN")
{
	'EICN',
	{
		6667, 0, 0, "Atlanta-r.GA.US.EarthInt.Net",
		6667, 0, 0, "ComBase.FL.US.EarthInt.Net",
		6667, 0, 0, "Shadow.FL.US.EarthInt.Net",
		6667, 0, 0, "Atlanta.GA.US.EarthInt.Net",
		6667, 0, 0, "Burghcom.PA.US.EarthInt.Net",
		6667, 0, 0, "Chicago.IL.US.EarthInt.Net",
		6667, 0, 0, "lafayette.la.us.earthint.net",
		6667, 0, 0, "Brigham.UT.US.EarthInt.Net"
	};
};

resource 'SRVR' (149, "WolfNet")
{
	'wolf',
	{
		6667, 0, 0, "wolfnet-irc.org",
		6667, 0, 0, "reston.va.us.wolfnet-irc.org",
		6667, 0, 0, "gorokan.nsw.au.wolfnet-irc.org",
		6667, 0, 0, "atlanta.ga.us.wolfnet-irc.org",
	};
};

resource 'SRVR' (150, "StarChat")
{
	'star',
	{
		6667, 0, 0, "Avenger.VA.US.StarChat.Net",
		6667, 0, 0, "Beer.VA.US.StarChat.Net",
		6667, 0, 0, "borg.UT.US.Starchat.Net",
		6667, 0, 0, "Cairns.QLD.AU.StarChat.Net",
		6667, 0, 0, "Darkness.CA.US.StarChat.Net",
		6667, 0, 0, "Durham-T.NC.US.StarChat.Net",
		6667, 0, 0, "Earth.MI.US.StarChat.Net",
		6667, 0, 0, "First.OH.US.StarChat.Net",
		6667, 0, 0, "FMC.GA.US.StarChat.Net",
		6667, 0, 0, "Hemulen.SE.EU.Starchat.Net",
		6667, 0, 0, "KeyWest.FL.US.StarChat.Net",
		6667, 0, 0, "Liquidnet.CA.US.StarChat.Net",
		6667, 0, 0, "LoneStar.TX.US.StarChat.Net",
		6667, 0, 0, "n0thing.AZ.US.StarChat.net",
		6667, 0, 0, "Nordic.SE.EU.Starchat.net",
		6667, 0, 0, "Palace.MS.US.StarChat.Net",
		6667, 0, 0, "Paradox.starchat.net",
		6667, 0, 0, "Polaris.WA.US.StarChat.Net",
		6667, 0, 0, "RebelBase.KL.MY.Starchat.Net",
		6667, 0, 0, "Severious.DC.US.StarChat.Net",
		6667, 0, 0, "shiva.uk.eu.starchat.net",
		6667, 0, 0, "Skyline.OR.US.StarChat.Net",
		6667, 0, 0, "Spider.QC.CA.StarChat.Net",
		6667, 0, 0, "Spooky.TX.US.StarChat.Net",
		6667, 0, 0, "StarGate.FL.US.StarChat.Net",
		6667, 0, 0, "utopia.uk.eu.starchat.net"
	};
};

resource 'SRVR' (151, "DarkTree.Net")
{
	'drkt',
	{
		6667, 0, 0, "Springsips.CO.DarkTree.Net",
		6667, 0, 0, "Tasmania.AU.Darktree.Net",
		6667, 0, 0, "Austin.TX.DarkTree.Net",
		6667, 0, 0, "Mastermind.AR.Darktree.Net",
		6667, 0, 0, "Cumulus.CO.DarkTree.Net",
		6667, 0, 0, "IronSide.CO.DarkTree.Net",
	};
};

resource 'SRVR' (152, "Open Projects Network")
{
	'opnn',
	{
		6667, 0, 0, "irc.openprojects.net"
	};
};

resource 'SRVR' (153, "QDeckers")
{
	'qdek',
	{
		6667, 0, 0, "irc.qdeckers.com",
		6667, 0, 0, "irc.shadowhawk.ro"
	};
};

/*
resource 'SRVR' (143, "EspritNet")
{
	'espr',
	{
	};
};

*/
