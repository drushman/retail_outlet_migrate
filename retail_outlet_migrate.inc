<?php

class RetailOutletMigrateMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('RetailOutlet'));
    
    $this->map = new MigrateSQLMap($this->machineName, 
        array('company' => array(
              'type' => 'varchar',
              'not null' => true,
              'length' => 255,
              ),
              'address' => array(
              'type' => 'varchar',
              'not null' => true,
              'length' => 255,
              ),
          )
        ,MigrateDestinationNode::getKeySchema()
    );
    
    $this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . drupal_get_path('module', 'retail_outlet_migrate') . '/csv/retailrcontactlist.csv', $this->csvcolumns(), array('header_rows' => 1));
    $this->destination = new MigrateDestinationNode('retail_outlet');

    
    module_load_include('inc', 'retail_outlet_migrate', 'retail_outlet_migrate.analytics');
    module_load_include('inc', 'retail_outlet_migrate', 'retail_outlet_migrate.image');
    $this->addFieldMapping('title', 'company');
    $this->addFieldMapping('field_store_address', 'temp');//->defaultValue('CA');//, 'address');
    $this->addFieldMapping('field_store_phone', 'storephone');
  }
  function csvcolumns() {
    // "nid", "Title", "Path", "Body"
    $columns[0] = array('company', 'Company');
    $columns[1] = array('storephone', 'StorePhone');
    $columns[2] = array('address', 'address');
    $columns[3] = array('province', 'Province');
    $columns[4] = array('city', 'City');
    $columns[5] = array('zcode', 'ZipCode');
    $columns[6] = array('storehours', 'StoreHours');
    $columns[7] = array('website', 'WebSite');
    $columns[8] = array('image', 'Image');
    return $columns;
  }
  function prepareRow($row) {
    $row->temp = "US";
  }
  function prepare($node,$row) {
    
    if (strlen($row->province) <= 3) {
      $country = $row->province;
      $area = '';
    }
    else {
      $country = '';
      $area = $this->getCountry($row->province, $country);
          
    }
    $node->field_store_address['und']['0']['element_key'] = "node|retail_outlet|field_store_address|und|0";
    $node->field_store_address['und']['0']['thoroughfare'] = $row->address;
    $node->field_store_address['und']['0']['administrative_area'] = $area;
    $node->field_store_address['und']['0']['postal_code'] = $row->zcode;
    $node->field_store_address['und']['0']['country'] = $country;
    $node->field_store_address['und']['0']['locality'] = $row->city;
    $table = table_opening_hours($row->storehours);
    if (isset($table)) {
      $node->field_opening_hours['en']['0']['tablefield'] = table_opening_hours($row->storehours);
      //print_r($node->field_opening_hours);
    }
    
    if (!empty($row->image)) {
      $path_parts = pathinfo($row->image);
      $remote_image_name = substr($path_parts['filename'],0 , 35);
      $remote_image_extension = $path_parts['extension'];
      $remote_image_path = $row->image;
      $file_path = file_default_scheme() . "://store-pictures";
      if (!file_prepare_directory($file_path, FILE_CREATE_DIRECTORY)) {
        drupal_mkdir($file_path);
      }
      write_image_info($remote_image_path, $remote_image_name, $remote_image_extension, $fid);
      $node->field_store_picture['und']['0']['fid'] = $fid;
      
    }
    if (!empty($row->website)) {
      $node->field_store_website['und']['0']['url'] = $row->website;
    }
    
    //print_r($row->storehours);
    
  }
  
  
  public function getCountry($city,&$country) {
    $US = array(
      ''   => '--',
      'AL' => 'Alabama',
      'AK' => 'Alaska',
      'AZ' => 'Arizona',
      'AR' => 'Arkansas',
      'CA' => 'California',
      'CO' => 'Colorado',
      'CT' => 'Connecticut',
      'DE' => 'Delaware',
      'DC' => 'District Of Columbia',
      'FL' => 'Florida',
      'GA' => 'Georgia',
      'HI' => 'Hawaii',
      'ID' => 'Idaho',
      'IL' => 'Illinois',
      'IN' => 'Indiana',
      'IA' => 'Iowa',
      'KS' => 'Kansas',
      'KY' => 'Kentucky',
      'LA' => 'Louisiana',
      'ME' => 'Maine',
      'MD' => 'Maryland',
      'MA' => 'Massachusetts',
      'MI' => 'Michigan',
      'MN' => 'Minnesota',
      'MS' => 'Mississippi',
      'MO' => 'Missouri',
      'MT' => 'Montana',
      'NE' => 'Nebraska',
      'NV' => 'Nevada',
      'NH' => 'New Hampshire',
      'NJ' => 'New Jersey',
      'NM' => 'New Mexico',
      'NY' => 'New York',
      'NC' => 'North Carolina',
      'ND' => 'North Dakota',
      'OH' => 'Ohio',
      'OK' => 'Oklahoma',
      'OR' => 'Oregon',
      'PA' => 'Pennsylvania',
      'RI' => 'Rhode Island',
      'SC' => 'South Carolina',
      'SD' => 'South Dakota',
      'TN' => 'Tennessee',
      'TX' => 'Texas',
      'UT' => 'Utah',
      'VT' => 'Vermont',
      'VA' => 'Virginia',
      'WA' => 'Washington',
      'WV' => 'West Virginia',
      'WI' => 'Wisconsin',
      'WY' => 'Wyoming',
      ' ' => '--',
      'AS' => 'American Samoa',
      'FM' => 'Federated States of Micronesia',
      'GU' => 'Guam',
      'MH' => 'Marshall Islands',
      'MP' => 'Northern Mariana Islands',
      'PW' => 'Palau',
      'PR' => 'Puerto Rico',
      'VI' => 'Virgin Islands',
    );
    
    
    //
    $IT  = array(
      ''   => t('--'),
      'AG' => 'Agrigento',
      'AL' => 'Alessandria',
      'AN' => 'Ancona',
      'AO' => 'Valle d\'Aosta/Vallée d\'Aoste',
      'AP' => 'Ascoli Piceno',
      'AQ' => 'L\'Aquila',
      'AR' => 'Arezzo',
      'AT' => 'Asti',
      'AV' => 'Avellino',
      'BA' => 'Bari',
      'BG' => 'Bergamo',
      'BI' => 'Biella',
      'BL' => 'Belluno',
      'BN' => 'Benevento',
      'BO' => 'Bologna',
      'BR' => 'Brindisi',
      'BS' => 'Brescia',
      'BT' => 'Barletta-Andria-Trani',
      'BZ' => 'Bolzano/Bozen',
      'CA' => 'Cagliari',
      'CB' => 'Campobasso',
      'CE' => 'Caserta',
      'CH' => 'Chieti',
      'CI' => 'Carbonia-Iglesias',
      'CL' => 'Caltanissetta',
      'CN' => 'Cuneo',
      'CO' => 'Como',
      'CR' => 'Cremona',
      'CS' => 'Cosenza',
      'CT' => 'Catania',
      'CZ' => 'Catanzaro',
      'EN' => 'Enna',
      'FC' => 'Forlì-Cesena',
      'FE' => 'Ferrara',
      'FG' => 'Foggia',
      'FI' => 'Firenze',
      'FM' => 'Fermo',
      'FR' => 'Frosinone',
      'GE' => 'Genova',
      'GO' => 'Gorizia',
      'GR' => 'Grosseto',
      'IM' => 'Imperia',
      'IS' => 'Isernia',
      'KR' => 'Crotone',
      'LC' => 'Lecco',
      'LE' => 'Lecce',
      'LI' => 'Livorno',
      'LO' => 'Lodi',
      'LT' => 'Latina',
      'LU' => 'Lucca',
      'MB' => 'Monza e Brianza',
      'MC' => 'Macerata',
      'ME' => 'Messina',
      'MI' => 'Milano',
      'MN' => 'Mantova',
      'MO' => 'Modena',
      'MS' => 'Massa-Carrara',
      'MT' => 'Matera',
      'NA' => 'Napoli',
      'NO' => 'Novara',
      'NU' => 'Nuoro',
      'OG' => 'Ogliastra',
      'OR' => 'Oristano',
      'OT' => 'Olbia-Tempio',
      'PA' => 'Palermo',
      'PC' => 'Piacenza',
      'PD' => 'Padova',
      'PE' => 'Pescara',
      'PG' => 'Perugia',
      'PI' => 'Pisa',
      'PN' => 'Pordenone',
      'PO' => 'Prato',
      'PR' => 'Parma',
      'PT' => 'Pistoia',
      'PU' => 'Pesaro e Urbino',
      'PV' => 'Pavia',
      'PZ' => 'Potenza',
      'RA' => 'Ravenna',
      'RC' => 'Reggio di Calabria',
      'RE' => 'Reggio nell\'Emilia',
      'RG' => 'Ragusa',
      'RI' => 'Rieti',
      'RM' => 'Roma',
      'RN' => 'Rimini',
      'RO' => 'Rovigo',
      'SA' => 'Salerno',
      'SI' => 'Siena',
      'SO' => 'Sondrio',
      'SP' => 'La Spezia',
      'SR' => 'Siracusa',
      'SS' => 'Sassari',
      'SV' => 'Savona',
      'TA' => 'Taranto',
      'TE' => 'Teramo',
      'TN' => 'Trento',
      'TO' => 'Torino',
      'TP' => 'Trapani',
      'TR' => 'Terni',
      'TS' => 'Trieste',
      'TV' => 'Treviso',
      'UD' => 'Udine',
      'VA' => 'Varese',
      'VB' => 'Verbano-Cusio-Ossola',
      'VC' => 'Vercelli',
      'VE' => 'Venezia',
      'VI' => 'Vicenza',
      'VR' => 'Verona',
      'VS' => 'Medio Campidano',
      'VT' => 'Viterbo',
      'VV' => 'Vibo Valentia',
    );
    
    //
    $BR = array(
      ''   => '--',
      'AC' => 'Acre',
      'AL' => 'Alagoas',
      'AM' => 'Amazonas',
      'AP' => 'Amapa',
      'BA' => 'Bahia',
      'CE' => 'Ceara',
      'DF' => 'Distrito Federal',
      'ES' => 'Espirito Santo',
      'GO' => 'Goias',
      'MA' => 'Maranhao',
      'MG' => 'Minas Gerais',
      'MS' => 'Mato Grosso do Sul',
      'MT' => 'Mato Grosso',
      'PA' => 'Para',
      'PB' => 'Paraiba',
      'PE' => 'Pernambuco',
      'PI' => 'Piaui',
      'PR' => 'Parana',
      'RJ' => 'Rio de Janeiro',
      'RN' => 'Rio Grande do Norte',
      'RO' => 'Rondonia',
      'RR' => 'Roraima',
      'RS' => 'Rio Grande do Sul',
      'SC' => 'Santa Catarina',
      'SE' => 'Sergipe',
      'SP' => 'Sao Paulo',
      'TO' => 'Tocantins',
    );
    
    $CA = array(
      ''   => '--',
      'AB' => 'Alberta',
      'BC' => 'British Columbia',
      'MB' => 'Manitoba',
      'NB' => 'New Brunswick',
      'NL' => 'Newfoundland',
      'NT' => 'Northwest Territories',
      'NS' => 'Nova Scotia',
      'NU' => 'Nunavut',
      'ON' => 'Ontario',
      'PE' => 'Prince Edward Island',
      'QC' => 'Quebec',
      'SK' => 'Saskatchewan',
      'YT' => 'Yukon Territory',
    );
    
    //
    $AU = array(
      ''   => '--',
      'ACT' => 'Australian Capital Territory',
      'NSW' => 'New South Wales',
      'NT' => 'Northern Territory',
      'QLD' => 'Queensland',
      'SA' => 'South Australia',
      'TAS' => 'Tasmania',
      'VIC' => 'Victoria',
      'WA' => 'Western Australia',
    );
    $LIB['AU'] = $AU; 
    $LIB['CA'] = $CA; 
    $LIB['BR'] = $BR; 
    $LIB['IT'] = $IT; 
    $LIB['US'] = $US; 
    
    foreach ($LIB as $country => $value) {
      foreach ($value as $code => $name) {
        $name = preg_replace('/\s+/', '', $name);
        $city = preg_replace('/\s+/', '', $city);
        if (strcmp($name, $city) == 0) {
          $country = $country;
          return $code;
        }
      }
    }
  }
}

