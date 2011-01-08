<!-- OAS SETUP BEGINS -->
           <!--
           //configuration
           OAS_query = '';
           OAS_target = '_new';
           //end of configuration
					<!--//set globals here-->
           OAS_version = 10;
           OAS_rn = '001234567890';
           OAS_rns = '1234567890';
           OAS_rn = new String (Math.random()); OAS_rns = OAS_rn.substring(2, 11);
           function OAS_NORMAL (pos) {
                       document.write('<a href=\"' + OAS_url + 'click_nx.ads/' + OAS_sitepage + '/1' + OAS_rns + '@' + OAS_listpos + '!' + pos + '?' + OAS_query + '\" target=' + OAS_target + '>');
                       document.write('<img src=\"' + OAS_url + 'adstream_nx.ads/' + OAS_sitepage + '/1' + OAS_rns + '@' + OAS_listpos + '!' + pos + '?' + OAS_query + '\" /></a>');
           }
           //-->
           </script>

           <script language='JavaScript1.1'>
           <!--
           OAS_version = 11;
           if (navigator.userAgent.indexOf('Mozilla/3') != -1 || navigator.userAgent.indexOf('Mozilla/4.0 WebTV') != -1)
                       OAS_version = 10;
           if (OAS_version >= 11)
                       document.write('<scr' + 'ipt language='JavaScript1.1' src=\"' + OAS_url + 'adstream_mjx.ads/' + OAS_sitepage + '/1' + OAS_rns + '@' + OAS_listpos + '?' + OAS_query + '\"><\/script>');//-->
           </script>

           <script language='JavaScript' type='text/javascript'>
           <!--
           document.write('');
           function OAS_AD(pos) {
                       if (OAS_version >= 11)
                                   OAS_RICH(pos);
                       else
                                   OAS_NORMAL(pos);
           }
           //-->

           <!-- OAS SETUP END -->