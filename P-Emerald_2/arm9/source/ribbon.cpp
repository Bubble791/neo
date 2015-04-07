#include "ribbon.h"
#include <initializer_list>

std::vector<u8> ribbon::getRibbons( const pokemon& p_pokemon ) {
    std::vector<u8> res;
    u8 curr = 0;
    for( u8 j : { p_pokemon.m_boxdata.m_ribbons2[ 0 ],
         p_pokemon.m_boxdata.m_ribbons2[ 1 ],
         p_pokemon.m_boxdata.m_ribbons2[ 2 ],
         p_pokemon.m_boxdata.m_ribbons2[ 3 ],

         p_pokemon.m_boxdata.m_ribbons2[ 0 ],
         p_pokemon.m_boxdata.m_ribbons2[ 1 ],
         p_pokemon.m_boxdata.m_ribbons2[ 2 ],
         p_pokemon.m_boxdata.m_ribbons2[ 3 ],

         p_pokemon.m_boxdata.m_ribbons2[ 0 ],
         p_pokemon.m_boxdata.m_ribbons2[ 1 ],
         p_pokemon.m_boxdata.m_ribbons2[ 2 ],
         p_pokemon.m_boxdata.m_ribbons2[ 3 ] } )
         for( u8 i = 0; i < 8; ++i, ++curr )
             if( j & ( 1 << i ) )
                 res.push_back( curr );
    return res;
}

ribbon RibbonList[ MAX_RIBBONS ] = {
    //0x3c (Contest Ribbons, Hoenn)
    { "Band der Coolness", "Sieger in der Normal-Klasse!" },
    { "Band der Coolness Super", "Sieger in der Super-Klasse!" },
    { "Band der Coolness Hyper", "Sieger in der Hyper-Klasse!" },
    { "Band der Coolness Master", "Sieger in der Master-Klasse!" },

    { "Band der Sch�nheit", "Sieger in der Normal-Klasse!" },
    { "Band der Sch�nheit Super", "Sieger in der Super-Klasse!" },
    { "Band der Sch�nheit Hyper", "Sieger in der Hyper-Klasse!" },
    { "Band der Sch�nheit Master", "Sieger in der Master-Klasse!" },

    //0x3d
    { "Band der Anmut", "Sieger in der Normal-Klasse!" },
    { "Band der Anmut Super", "Sieger in der Super-Klasse!" },
    { "Band der Anmut Hyper", "Sieger in der Hyper-Klasse!" },
    { "Band der Anmut Master", "Sieger in der Master-Klasse!" },

    { "Band der Klugheit", "Sieger in der Normal-Klasse!" },
    { "Band der Klugheit Super", "Sieger in der Super-Klasse!" },
    { "Band der Klugheit Hyper", "Sieger in der Hyper-Klasse!" },
    { "Band der Klugheit Master", "Sieger in der Master-Klasse!" },

    //0x3e
    { "Band der St�rke", "Sieger in der Normal-Klasse!" },
    { "Band der St�rke Super", "Sieger in der Super-Klasse!" },
    { "Band der St�rke Hyper", "Sieger in der Hyper-Klasse!" },
    { "Band der St�rke Master", "Sieger in der Master-Klasse!" },

    { "Band des Champs von Hoenn", "Band f�r das Erreichen des Titels Champ und den Eintritt in die Ruhmeshalle." },
    { "Band des Gewinners", "Band f�r Siege in der Lv. 50-Kategorie im Duellturm von Hoenn." },
    { "Band des Sieges", "Band f�r Siege in der Lv. 100-Kategorie im Duellturm von Hoenn." },
    { "Band des K�nstlers", "Belohnung f�r die Wahl zum Super-Model in Hoenn." },

    //0x3f
    { "Flei�-Band", "Band f�r extrem flei�ige Arbeiter." },
    { "Band des Meeres", "" },
    { "Band der Landmassen", "" },
    { "Band der L�fte", "" },

    { "Band des Landes", "Pok�mon Liga Band des Champs" },
    { "Band des Nation", "Band f�r den erfolgreichen Abschluss aller Schwierigkeitsgrade." },
    { "Band der Erde", "100. Sieg in Folge Gedenk-Band" },
    { "Band der Welt", "Pok�mon Liga Band des Champs" },

    // 0x24
    { "Band des Champs von Sinnoh", "Band f�r den Sieg �ber den Sinnoh Champ und den Eintritt in die Ruhmeshalle." },
    { "Band der F�higkeit", "Ein Band, verliehen zum Sieg �ber den Kampfkoloss des Duellturms." },
    { "Gro�es Band der F�higkeit", "Ein Band, verliehen zum Sieg �ber den Kampfkoloss des Duellturms." },
    { "Doppel-Band der F�higkeit", "Ein Band f�r die Meisterung des Doppels im Sinnoh Duellturm." },

    { "Multi-Band der F�higkeit", "Ein Band f�r die Meisterung der Multi-Herausforderung im Sinnoh Duellturm." },
    { "Paar-Band der F�higkeit", "Ein Band f�r die Meisterung der Link-Multi-Herausforderung im Sinnoh Duellturm." },
    { "Welt-Band der F�higkeit", "Ein Band f�r die Meisterung der Wi-Fi-Herausforderung im Sinnoh Duellturm." },
    { "Band der Wachsamkeit", "Ein Band zur Erinnerung an ein st�rkendes, Lebensenergie gebendes Erlebnis." },

    //0x25
    { "Band des Schocks", "Ein Band zur Erinnerung an ein Ereignis, das das Leben aufregender machte." },
    { "Band des Niederschlags", "Ein Band zur Erinnerung an die Traurigkeit, die dem Leben W�rze gab." },
    { "Band der Sorglosigkeit", "Ein Band zur Erinnerung an einen Fehler, der zu lebenswichtigen Entscheidungen f�hrte." },
    { "Band der Entspannung", "Ein Band zur Erinnerung an ein erfrischendes Ereignis, das dem Leben Glanz gab." },

    { "Band des Schlafens", "Ein Band zur Erinnerung an einen tiefen Schlaf, der den Fluss des Lebens beruhigte." },
    { "Band des L�chelns", "Ein Band zur Erinnerung daran, dass ein L�cheln das Leben bereichert." },
    { "Hinrei�endes Band", "Ein au�ergew�nlich sch�nes und extravagantes Band." },
    { "K�nigliches Band", "Ein unglaublich k�nigliches Band, das eine Aura der Erhabenheit ausstrahlt." },

    //0x26
    { "Hinrei�endes K�nigliches Band", "Ein wundersch�nes und k�nigliches Band, das einfach sagenhaft ist." },
    { "Fu�abdruck-Band", "Ein Band f�r ein Pok�mon, das einen besonders guten Fu�abdruck hinterl�sst." },
    { "Band des Rekords", "Ein Band f�r das Aufstellen eines unglaublichen Rekords." },
    { "Band der Geschichte", "Ein Band f�r das Aufstellen eines historischen Rekords." },

    { "Band der Legende", "Ein Band f�r das Aufstellen eines legend�ren Rekords." },
    { "Rotes Band", "" },
    { "Gr�nes Band", "" },
    { "Blaues Band", "" },

    //0x27
    { "Festival-Band", "Pok�mon-Festival Teilnehmer-Band" },
    { "Jahrmarkt-Band", "" },
    { "Klassisches Band", "" },
    { "Premierband", "" },

    { "Sonderband", "Besonderes Band f�r einen besonderen Tag." },
    { "Kampfmeisterband", "Band des Siegers eines Kampfturniers." },
    { "", "" },
    { "", "" },

    //0x60 (Contest Ribbons, Sinnoh)
    { "Band der Coolness", "Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Coolness Mega", "Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Coolness Ultra", "Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Coolness Master", "Sieger in der Master-Klasse in Sinnoh!" },

    { "Band der Sch�nheit", "Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Sch�nheit Mega", "Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Sch�nheit Ultra", "Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Sch�nheit Master", "Sieger in der Master-Klasse in Sinnoh!" },

    //0x61
    { "Band der Anmut", "Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Anmut Mega", "Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Anmut Ultra", "Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Anmut Master", "Sieger in der Master-Klasse in Sinnoh!" },

    { "Band der Klugheit", "Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Klugheit Mega", "Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Klugheit Ultra", "Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Klugheit Master", "Sieger in der Master-Klasse in Sinnoh!" },

    //0x62
    { "Band der St�rke", "Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der St�rke Mega", "Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der St�rke Ultra", "Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der St�rke Master", "Sieger in der Master-Klasse in Sinnoh!" },

    { "", "" },
    { "", "" },
    { "", "" },
    { "", "" },

    //0x63
    { "", "" },
    { "", "" },
    { "", "" },
    { "", "" },

    { "", "" },
    { "", "" },
    { "", "" },
    { "", "" },
};